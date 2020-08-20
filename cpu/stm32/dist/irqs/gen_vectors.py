#!/bin/env python3

# Copyright (C) 2020 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""Generate the vectors table for a given STM32 CPU line."""

import os
import argparse

from jinja2 import Environment, FileSystemLoader


CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
RIOTBASE = os.getenv(
    "RIOTBASE", os.path.abspath(os.path.join(CURRENT_DIR, "../../..")))
STM32_VECTORS_DIR = os.path.join(RIOTBASE, "cpu/stm32/vectors")
STM32_CMSIS_FILE = os.path.join(
    RIOTBASE, "cpu/stm32/include/vendor/cmsis/{}/Include/{}.h")


def parse_cmsis(cpu_line):
    """Parse the CMSIS to get the list IRQs."""
    cpu_fam = cpu_line[5:7]
    cpu_line_cmsis = STM32_CMSIS_FILE.format(cpu_fam.lower(), cpu_line.lower())

    with open(cpu_line_cmsis, 'rb') as cmsis:
        cmsis_content = cmsis.readlines()

    irq_lines = []
    use_line = False
    for line in cmsis_content:
        try:
            line = line.decode()
        except UnicodeDecodeError:
            # skip line that contains non unicode characters
            continue
        # start filling lines after interrupt Doxygen comment
        if line.startswith("typedef enum"):
            use_line = True
        # skip useless lines
        if (
            # Skip lines with comments
            line.startswith("/*") or
            # Skip lines with negative IRQs indices
            "= -" in line or
            # Skip line with enum opening brace
            line.startswith("{") or
            # Skip line defining the enum
            line.startswith("typedef enum") or
            # Skip line starting with too many spaces
            line.startswith(" " * 8) or
            # Skip remapped USB interrupt
            # (set as alias for USBWakeUp_IRQn on stm32f3)
            "USBWakeUp_RMP_IRQn" in line
        ):
            continue

        # Stop at the end of the IRQn_Type enum definition
        if "IRQn_Type" in line:
            break

        # Only append IRQ line if it should be used and is not empty
        irq_line = line.split("=")[0].strip()
        if use_line and irq_line:
            irq_lines.append(irq_line)

    isrs = [
        {
            "irq": irq,
            "func": "exti" if "EXTI" in irq else irq.lower().rsplit("_", 1)[0]
        }
        for irq in irq_lines
    ]

    return {"isrs": isrs, "cpu_line": cpu_line}


def generate_vectors(context):
    """Use Jinja2 template to generate the vectors C file."""
    loader = FileSystemLoader(searchpath=CURRENT_DIR)
    env = Environment(
        loader=loader, trim_blocks=True, lstrip_blocks=True,
        keep_trailing_newline=True
    )
    env.globals.update(zip=zip)
    template = env.get_template("template_vectors.c.j2")
    render = template.render(**context)

    dest_file = os.path.join(
        STM32_VECTORS_DIR, "gen_{}.c".format(context["cpu_line"])
    )

    with open(dest_file, "w") as f_dest:
        f_dest.write(render)


def main(args):
    """Main function."""
    context = parse_cmsis(args.cpu_line)
    generate_vectors(context)


PARSER = argparse.ArgumentParser()
PARSER.add_argument("cpu_line", help="STM32 CPU line")

if __name__ == "__main__":
    main(PARSER.parse_args())
