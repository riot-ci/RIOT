include $(RIOTBOARD)/common/makefiles/stdio_cdc_acm.dep.mk

# enable bootloader reset over USB, requires USB bootloader to be used
ifneq (,$(filter stdio_cdc_acm,$(USEMODULE)))
  FEATURES_REQUIRED += $(BOOTLOADER_TYPE)
  USEMODULE += usb_board_reset
endif
