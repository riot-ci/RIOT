## Overview

Vera++ is a programmable tool for verification, analysis and transformation of
C++ source code. Vera++ is mainly an engine that parses C++ source files and
presents the result of this parsing to scripts in the form of various
collections - the scripts are actually performing the requested tasks.

## Rules

Rules can be added in the `rules` folder. The default rules from Vera++ were
added. However, it's possible to implement new rules in TCL. See
https://bitbucket.org/verateam/vera/wiki/Rules for information about the
available rules.

# Profiles

A `riot` profile was made for describing the set of applied rules as well as
special parameters. In case new rules are added, they can be updated under
`profiles/riot` file.

## Example usage
Check all changed files changed in current branch:
    check.sh

Check all changed files and don't display the results (only exit code):
    QUIET=1 check.sh

Check all changed files and only display errors (exit code 0):
    WARNONLY=1 check.sh 

Use a different profile under `profile` folder:
    VERA_PROFILE=other_profile check.sh

