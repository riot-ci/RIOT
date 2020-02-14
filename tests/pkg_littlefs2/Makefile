include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove \
                             arduino-leonardo \
                             arduino-nano \
                             arduino-uno \
			     atmega328p \
                             i-nucleo-lrwan1 \
                             nucleo-f031k6 \
                             nucleo-f042k6 \
                             nucleo-l031k6 \
                             nucleo-l053r8 \
                             nucleo-f030r8 \
			     stm32f030f4-demo\
                             stm32f0discovery \
                             stm32l0538-disco \
			     waspmote-pro \

# Set vfs file and dir buffer sizes
CFLAGS += -DVFS_FILE_BUFFER_SIZE=84 -DVFS_DIR_BUFFER_SIZE=52
# Reduce LFS_NAME_MAX to 31 (as VFS_NAME_MAX default)
CFLAGS += -DLFS_NAME_MAX=31

USEPKG += littlefs2
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
