#!/bin/env python3

# Copyright (C) 2020 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""Generate a header with the number of IRQs for a given STM32 family."""

import os
import argparse
import re

from jinja2 import Environment, FileSystemLoader


CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
RIOTBASE = os.getenv(
    "RIOTBASE", os.path.abspath(os.path.join(CURRENT_DIR, "../../../..")))
STM32_INCLUDE_DIR = os.path.join(RIOTBASE, "cpu/stm32/include")
STM32_CMSIS_INCLUDE_DIR = os.path.join(
    RIOTBASE, STM32_INCLUDE_DIR, "vendor/cmsis/{}/Include/")
STM32_CMSIS_HEADER = os.path.join(
    RIOTBASE, STM32_CMSIS_INCLUDE_DIR, "{}.h")
STM32_IRQS_DIR = os.path.join(
    RIOTBASE, STM32_INCLUDE_DIR, "irqs/{}/irqs.h")


def list_cpu_lines(cpu_fam):
    """Returns the list CPU lines for a given family"""
    headers = os.listdir(STM32_CMSIS_INCLUDE_DIR.format(cpu_fam))
    headers.remove("stm32{}xx.h".format(cpu_fam))
    headers.remove("system_stm32{}xx.h".format(cpu_fam))
    return [header.split(".")[0] for header in headers]


def irq_numof(cpu_fam, cpu_line):
    """Parse the CMSIS to get the list IRQs."""
    cpu_line_cmsis = STM32_CMSIS_HEADER.format(cpu_fam, cpu_line)

    with open(cpu_line_cmsis, 'rb') as cmsis:
        cmsis_content = cmsis.readlines()

    for line_idx, line in enumerate(cmsis_content):
        try:
            line = line.decode()
        except UnicodeDecodeError:
            # skip line that contains non unicode characters
            continue
        # skip useless lines
        if (
            # Skip lines with comments
            line.startswith("/*") or
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

    # Ensure we are on a valid line, otherwise search in earlier lines
    last_irq_line = cmsis_content[line_idx - 1].decode()
    while last_irq_line.startswith(" " * 8):
        line_idx -= 1
        last_irq_line = cmsis_content[line_idx - 1].decode()

    # Use a regexp to find the last IRQ line index
    irq_numof_match = re.search(r"(= \d+)", last_irq_line).group(0)

    return int(irq_numof_match.replace("= ", "").strip()) + 1


def generate_irqs(cpu_fam, context):
    """Use Jinja2 template to generate the irqs.h file."""
    loader = FileSystemLoader(searchpath=CURRENT_DIR)
    env = Environment(
        loader=loader, trim_blocks=True, lstrip_blocks=True,
        keep_trailing_newline=True
    )
    env.globals.update(zip=zip)
    template = env.get_template("template_irqs.h.j2")
    render = template.render(**context)

    dest_file = os.path.join(STM32_IRQS_DIR.format(cpu_fam))

    dest_dir = os.path.dirname(dest_file)
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)

    with open(dest_file, "w") as f_dest:
        f_dest.write(render)


def main(args):
    """Main function."""
    cpu_lines = list_cpu_lines(args.cpu_fam)
    context = {
        "cpu_fam": args.cpu_fam,
        "cpu_lines": [
            {
                "line": cpu_line.upper().replace("X", "x"),
                "irq_numof": irq_numof(args.cpu_fam, cpu_line),
            }
            for cpu_line in cpu_lines
        ]
    }
    generate_irqs(args.cpu_fam, context)


PARSER = argparse.ArgumentParser()
PARSER.add_argument("cpu_fam", help="STM32 CPU Family")

if __name__ == "__main__":
    main(PARSER.parse_args())
