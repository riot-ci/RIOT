#!/usr/bin/env python3

# Copyright (C) 2020 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""Generate the vectors table for a given STM32 CPU line."""

import os
import argparse


CURRENT_DIR = os.path.dirname(os.path.abspath(__file__))
RIOTBASE = os.getenv(
    "RIOTBASE", os.path.abspath(os.path.join(CURRENT_DIR, "../../../..")))
STM32_VECTORS_DIR = os.path.join(RIOTBASE, "cpu/stm32/vectors")
STM32_CMSIS_FILE = os.path.join(
    RIOTBASE, "cpu/stm32/include/vendor/cmsis/{}/Include/{}.h")

VECTORS_FORMAT = """
/*
 * PLEASE DON'T EDIT
 *
 * This file was automatically generated by
 * ./cpu/stm32/dist/irqs/gen_vectors.py
 */

#include "vectors_cortexm.h"

/* define a local dummy handler as it needs to be in the same compilation unit
 * as the alias definition */
void dummy_handler(void) {{
    dummy_handler_default();
}}

/* {cpu_line} specific interrupt vectors */
{isr_lines}

/* CPU specific interrupt vector table */
ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {{
{irq_lines}
}};
"""


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
    """Use vector template string to generate the vectors C file."""
    isr_line_format = "WEAK_DEFAULT void isr_{func}(void);"
    irq_line_format = "    [{irq:<35}] = isr_{func},"

    isr_lines = []
    irq_lines = []
    for isr in context["isrs"]:
        isr_line = isr_line_format.format(**isr)
        if isr_line not in isr_lines:
            isr_lines.append(isr_line)
        irq_lines.append(irq_line_format.format(**isr))
    vectors_content = VECTORS_FORMAT.format(
        cpu_line=context["cpu_line"],
        isr_lines="\n".join(isr_lines),
        irq_lines="\n".join(irq_lines),
    )

    dest_file = os.path.join(
        STM32_VECTORS_DIR, "gen_{}.c".format(context["cpu_line"])
    )

    with open(dest_file, "w") as f_dest:
        f_dest.write(vectors_content)


def main(args):
    """Main function."""
    context = parse_cmsis(args.cpu_line)
    generate_vectors(context)


PARSER = argparse.ArgumentParser()
PARSER.add_argument("cpu_line", help="STM32 CPU line")

if __name__ == "__main__":
    main(PARSER.parse_args())
