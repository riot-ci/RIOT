APPLICATION = test_not_defining_riotbase

# Only one board for testing on CI
BOARD_WHITELIST := native

BOARD ?= native

# Not defining 'RIOTBASE' this should be handled by the main Makefile.include
#
# However it does not work if there is a Makefile.local
include ../../Makefile.include
