include ../Makefile.tests_common

# HACK were disabled for `armv7` in unittests and fail to pass the test
BOARD_BLACKLIST += avsextrem
BOARD_BLACKLIST += msba2

USEMODULE += embunit

FEATURES_REQUIRED += cpp

DISABLE_MODULE += auto_init
TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
