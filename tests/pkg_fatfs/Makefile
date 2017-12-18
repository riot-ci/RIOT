include ../Makefile.tests_common

BOARD ?= native

FEATURES_OPTIONAL += periph_rtc

# whitelist can be removed when the problem described in #6063 was resolved
# this list is composed of boards that support spi + native
BOARD_WHITELIST := native airfy-beacon arduino-due arduino-duemilanove arduino-mega2560 \
                   arduino-uno arduino-zero avsextrem cc2538dk fox frdm-k64f iotlab-a8-m3 \
                   iotlab-m3 limifrog-v1 maple-mini msb-430 msb-430h msba2 msbiot mulle \
                   nrf52840dk nrf52dk nrf6310 nucleo-f072 nucleo-f091 nucleo-f103 \
                   nucleo-f302 nucleo-f303 nucleo-f334 nucleo-f401 nucleo-f410 nucleo-f411 \
                   nucleo-f446 nucleo-l053 nucleo-l073 nucleo-l152 nucleo-l476 nucleo144-f207 \
                   nucleo144-f303 nucleo144-f413 nucleo144-f429 nucleo144-f446 nucleo32-f031 \
                   nucleo32-f042 nucleo32-f303 nucleo32-l031 nucleo32-l432 openmote-cc2538 \
                   pba-d-01-kw2x remote-pa remote-reva remote-revb samd21-xpro \
                   saml21-xpro samr21-xpro sodaq-autonomo spark-core stm32f0discovery \
                   stm32f3discovery stm32f4discovery telosb udoo waspmote-pro \
                   wsn430-v1_3b wsn430-v1_4 yunjia-nrf51822 z1

USEMODULE += shell
USEMODULE += fatfs_diskio_mtd
USEMODULE += mtd
USEPKG    += fatfs

FATFS_IMAGE_FILE_SIZE_MIB ?= 128

ifeq ($(BOARD),native)
#overwrite default mtd_native-config to use fat image as flash device
CFLAGS += -DMTD_NATIVE_FILENAME=\"./bin/riot_fatfs_disk.img\"
CFLAGS += -DMTD_NATIVE_PAGE_SIZE=512
CFLAGS += -DMTD_NATIVE_SECTOR_SIZE=512
CFLAGS += -DFATFS_IMAGE_FILE_SIZE_MIB=$(FATFS_IMAGE_FILE_SIZE_MIB)
CFLAGS += -DMTD_NATIVE_SECTOR_NUM=\(\(\(FATFS_IMAGE_FILE_SIZE_MIB\)*1024*1024\)/MTD_NATIVE_SECTOR_SIZE\)
else
# for actual hardware use mtd_sdcard as storage device
USEMODULE += mtd_sdcard
endif

image:
	@tar -xjf riot_fatfs_disk.tar.gz -C ./bin/

#this generates a compressed fat image file that can be used by the fat driver on native
compressed-image:
	@./create_fat_image_file.sh $(FATFS_IMAGE_FILE_SIZE_MIB)

include $(RIOTBASE)/Makefile.include
