# Debug targets to evaluate dependency migrations
#
# The goal is to get the value of variables used for dependency resolution
# and the impact of refactoring

# Files output can be generated through `info-boards-supported` evaluation with
#
#     DEPENDENCY_DEBUG=1 make info-boards-supported
#
# And for each board in the normal make context with
#
#     BOARD=board_name make dependency_debug
#
# To compare in an aggregated file, you can run in an application directory:
#
#    for board in $(make info-boards); do DEPENDENCY_DEBUG_OUTPUT_DIR=bin/info BOARD=${board} make dependency_debug; done; cat bin/info/* > bin/deps_info
#    DEPENDENCY_DEBUG=1 DEPENDENCY_DEBUG_OUTPUT_DIR=bin/info-global make info-boards-supported; cat bin/info-global/* > bin/deps_info-boards-supported
#    # And compare both files
#    diff -u bin/deps_info bin/deps_info-boards-supported
#
# And when comparing two revisions, include the revision in the file names

.PHONY: dependency_debug
dependency_debug:
	$(call file_save_dependencies_variables,dependencies_info)
	@:

DEPENDENCY_DEBUG_OUTPUT_DIR ?= $(CURDIR)

# Save variables that are used for parsing dependencies
_DEPS_DEBUG_VARS += BOARD CPU CPU_MODEL CPU_FAM
_DEPS_DEBUG_VARS += FEATURES_PROVIDED FEATURES_REQUIRED FEATURES_OPTIONAL FEATURES_USED FEATURES_MISSING FEATURES_CONFLICT FEATURES_CONFLICTING
_DEPS_DEBUG_VARS += USEMODULE DEFAULT_MODULE DISABLE_MODULE
DEPS_DEBUG_VARS ?= $(_DEPS_DEBUG_VARS)

# Put some dummy first line to be sure the file is create from scratch
file_save_dependencies_variables = $(call file_save_variable,$(DEPENDENCY_DEBUG_OUTPUT_DIR)/$1_$(BOARD),$(DEPS_DEBUG_VARS))
file_save_variable = $(shell mkdir -p $(dir $1); rm -f $1)$(foreach v,$2,$(file >>$1,$(call _print_var,$v)))

# Remove spaces in case of empty value
# Remove spaces around value as it happens
_print_var = $(strip $1 = $(strip $($1)))
