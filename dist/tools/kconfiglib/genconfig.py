#!/usr/bin/env python3

# Copyright (c) 2018-2019, Ulf Magnusson
#               2020 HAW Hamburg
# SPDX-License-Identifier: ISC

"""
This script is used to merge multiple configuration sources and generate
different outputs related to Kconfig:

- Generate a header file with #defines from the configuration, matching the
format of include/generated/autoconf.h in the Linux kernel.

- Write the configuration output as a .config file. See --config-out.

- The --sync-deps, --file-list, and --env-list options generate information that
can be used to avoid needless rebuilds/reconfigurations.

Before writing a header or configuration file, Kconfiglib compares the old
contents of the file against the new contents. If there's no change, the write
is skipped. This avoids updating file metadata like the modification time, and
might save work depending on your build setup.

A custom header string can be inserted at the beginning of generated
configuration and header files by setting the KCONFIG_CONFIG_HEADER and
KCONFIG_AUTOHEADER_HEADER environment variables, respectively. The string is
not automatically made a comment (this is by design, to allow anything to be
added), and no trailing newline is added, so add '/* */', '#', and newlines as
appropriate.
"""
import argparse
import logging
import os
import sys

import kconfiglib


DEFAULT_SYNC_DEPS_PATH = "deps/"


class NoConfigurationFile(Exception):
    """
    Raised when an operation that requires a configuration input file is
    executed but the file is not specified.
    """
    pass


def is_module(symbol):
    """
    Checks if a given symbol represents a module, depending on its prefix.
    """
    return symbol.name.startswith("MODULE_")

def is_error(symbol):
    """
    Checks if a given symbol represents an error, depending on its prefix.
    """
    return symbol.name.startswith("ERROR_")

def merge_configs(kconf, configs=[]):
    # Enable warnings for assignments to undefined symbols
    kconf.warn_assign_undef = True

    # (This script uses alldefconfig as the base. Other starting states could be
    # set up here as well. The approach in examples/allnoconfig_simpler.py could
    # provide an allnoconfig starting state for example.)

    # Disable warnings generated for multiple assignments to the same symbol within
    # a (set of) configuration files. Assigning a symbol multiple times might be
    # done intentionally when merging configuration files.
    kconf.warn_assign_override = False
    kconf.warn_assign_redun = False

    # Create a merged configuration by loading the fragments with replace=False.
    for config in configs:
        logging.debug(kconf.load_config(config, replace=False))


def check_configs(kconf, check_app_sym=False):
    """
    Verifies that the generated configuration is valid.

    A configuration is not valid when:
        - An error symbol has been set because of an invalid condition.
        - A module could not be set to the value defined by the user.
        - A configuration parameter could not be set to value defined by the
          user.
    """
    for sym in kconf.unique_defined_syms:
        if is_error(sym) and sym.str_value:
            logging.error("{}".format(sym.str_value))
            return False

        elif check_app_sym and sym.name == "APPLICATION" and sym.str_value == "n":
            logging.error("The dependencies for the application are not"
                          " met:\n{}".format(sym))
            return False

        # Was the symbol assigned to?
        elif sym.user_value is not None:
            # Tristate values are represented as 0, 1, 2. Having them as
            # "n", "m", "y" is more convenient here, so convert.
            if sym.type in (kconfiglib.BOOL, kconfiglib.TRISTATE):
                user_value = kconfiglib.TRI_TO_STR[sym.user_value]
            else:
                user_value = sym.user_value

            if user_value != sym.str_value:
                logging.debug("{} was assigned the value '{}' but got the value"
                              " '{}'. Check the dependencies".format(sym.name,
                              user_value, sym.str_value))
                if is_module(sym):
                    logging.error("The module {} could not be set to {}."
                                  " Check the dependencies.\n{}".format(
                                        sym.name,
                                        kconfiglib.TRI_TO_STR[sym.user_value],
                                        sym
                                    ))
                    return False
                else:
                    logging.error("The parameter {} could not be set to {}."
                                  "\n{}".format(sym.name, sym.user_value, sym))
                    return False
    return True


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=__doc__)

    parser.add_argument(
        "--header-path",
        metavar="HEADER_FILE",
        help="""
Path to write the generated header file to. If not specified the header file is
not written.
""")

    parser.add_argument(
        "--config-out",
        metavar="CONFIG_FILE",
        help="""
Write the configuration to CONFIG_FILE. If not specified the file is not
written.
""")

    parser.add_argument(
        "--kconfig-filename",
        metavar="KCONFIG_FILENAME",
        nargs="?",
        default="Kconfig",
        help="Top-level Kconfig file (default: Kconfig)")

    parser.add_argument(
        "--sync-deps",
        metavar="OUTPUT_DIR",
        nargs="?",
        const=DEFAULT_SYNC_DEPS_PATH,
        help="""
Enable generation of symbol dependency information for incremental builds,
optionally specifying the output directory (default: {}). See the docstring of
Kconfig.sync_deps() in Kconfiglib for more information.
""".format(DEFAULT_SYNC_DEPS_PATH))

    parser.add_argument(
        "--file-list",
        metavar="FILE_LIST_FILE",
        help="""
Write a makefile listing all the Kconfig files used, and adding them as
dependencies of HEADER_FILE. The paths are absolute. Files appear in the order
they're 'source'd.
""")

    parser.add_argument(
        "--env-list",
        metavar="ENV_LIST_FILE",
        help="""
Write a list of all environment variables referenced in Kconfig files to
ENV_LIST_FILE, with one variable per line. Each line has the format NAME=VALUE.
Only environment variables referenced with the preprocessor $(VAR) syntax are
included, and not variables referenced with the older $VAR syntax (which is
only supported for backwards compatibility).
""")

    parser.add_argument(
        "--check-app-sym",
        action='store_true',
        help="Check that, when defined, the 'APPLICATION' symbol is set."
    )

    parser.add_argument(
        "-d", "--debug",
        action="store_true",
        help="Enable debug messages")

    parser.add_argument(
        "--config-sources",
        metavar="CONFIG_SOURCES",
        nargs='*',
        help="List of configuration files to merge and apply. May be empty.")

    args = parser.parse_args()
    log_level = logging.DEBUG if args.debug else logging.ERROR
    logging.basicConfig(format='[genconfig.py]:%(levelname)s-%(message)s',
                        level=log_level)

    kconf = kconfiglib.Kconfig(args.kconfig_filename)
    merge_configs(kconf, args.config_sources)

    if not check_configs(kconf, check_app_sym=args.check_app_sym):
        sys.exit(1)

    if args.config_out is not None:
        logging.debug(kconf.write_config(args.config_out, save_old=False))

    if args.header_path is not None:
        logging.debug(kconf.write_autoconf(args.header_path))

    if args.sync_deps is not None:
        logging.debug("Incremental build header files generated at '{}'".format(args.sync_deps))
        kconf.sync_deps(args.sync_deps)

    if args.file_list is not None:
        if args.config_out is None:
            raise NoConfigurationFile("Can't generate Kconfig dependency file without configuration file")
        logging.debug("Kconfig dependencies written to '{}'".format(args.file_list))
        with open(args.file_list, "w", encoding="utf-8") as f:
            f.write("{}: \\\n".format(args.config_out))
            # add dependencies
            for path in kconf.kconfig_filenames:
                f.write("    {} \\\n".format(os.path.abspath(path)))
            # add empty recipes for dependencies
            f.write("\n\n")
            for path in kconf.kconfig_filenames:
                f.write("{}:\n\n".format(os.path.abspath(path)))

    if args.env_list is not None:
        logging.debug("Kconfig environmental variables written to '{}'".format(args.env_list))
        with open(args.env_list, "w", encoding="utf-8") as f:
            for env_var in kconf.env_vars:
                f.write("{}={}\n".format(env_var, os.environ[env_var]))


if __name__ == "__main__":
    main()
