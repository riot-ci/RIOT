include ../Makefile.tests_common

BOARD ?= native

FEATURES_OPTIONAL += periph_rtc

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
