APPLICATION = pkg_fatfs_vfs
include ../Makefile.tests_common

USEMODULE += fatfs_vfs
FEATURES_OPTIONAL += periph_rtc

CFLAGS += -DVFS_FILE_BUFFER_SIZE=72 -DVFS_DIR_BUFFER_SIZE=44

FATFS_IMAGE_FILE_SIZE_MIB ?= 128

ifeq ($(BOARD),native)
  USEMODULE += mtd_native

  #overwrite default mtd_native-config to use fat image as flash device
  MTD_NATIVE_FILENAME    ?= \"./bin/riot_fatfs_disk.img\"
  MTD_NATIVE_PAGE_SIZE   ?= 512
  MTD_NATIVE_SECTOR_SIZE ?= 512
  MTD_NATIVE_SECTOR_NUM  ?= \(\(\(FATFS_IMAGE_FILE_SIZE_MIB\)*1024*1024\)/MTD_NATIVE_SECTOR_SIZE\)
  CFLAGS += -DMTD_NATIVE_FILENAME=$(MTD_NATIVE_FILENAME)
  CFLAGS += -DMTD_NATIVE_PAGE_SIZE=$(MTD_NATIVE_PAGE_SIZE)
  CFLAGS += -DMTD_NATIVE_SECTOR_SIZE=$(MTD_NATIVE_SECTOR_SIZE)
  CFLAGS += -DFATFS_IMAGE_FILE_SIZE_MIB=$(FATFS_IMAGE_FILE_SIZE_MIB)
  CFLAGS += -DMTD_NATIVE_SECTOR_NUM=$(MTD_NATIVE_SECTOR_NUM)
else
  USEMODULE += mtd_sdcard
endif

include $(RIOTBASE)/Makefile.include

#this generates a compressed fat image file that can be used by the fat driver on native
fatimage:
	@./create_fat_image_file.sh $(FATFS_IMAGE_FILE_SIZE_MIB)

test:
	@tar -xjf riot_fatfs_disk.tar.gz -C ./bin/
	./tests/01-run.py
