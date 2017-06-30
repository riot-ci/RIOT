ifneq (,$(filter mcuboot, $(MAKECMDGOALS)))
IMGTOOL ?= $(RIOTBASE)/dist/tools/mcuboot/imgtool.py
override IMGTOOL := $(abspath $(IMGTOOL))

ifndef SLOT0_SIZE
$(error Board $(BOARD) does not define multislot parameters!)
else
export FLASH_OFFSET := $(SLOT0_SIZE)
endif

export BINFILE ?= $(BINDIR)/$(APPLICATION).bin
export SIGN_BINFILE = $(BINDIR)/signed-$(APPLICATION).bin
export OFLAGS = -O binary

create-key: key.pem

key.pem:
	$(Q)$(IMGTOOL) keygen -k key.pem -t rsa-2048

mcuboot: create-key all
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '${COLOR_PURPLE}Re-linking for MCUBoot at $(SLOT0_SIZE)...${COLOR_RESET}'
	@$(COLOR_ECHO)
	$(Q)$(_LINK) $(LINKFLAGPREFIX)--defsym=offset="$(SLOT0_SIZE)" \
	$(LINKFLAGPREFIX)--defsym=length="$(SLOT1_SIZE)" \
	$(LINKFLAGPREFIX)--defsym=image_header="$(IMAGE_HDR)" -o $(ELFFILE) && \
	$(OBJCOPY) $(OFLAGS) $(ELFFILE) $(BINFILE) && \
	$(IMGTOOL) sign --key key.pem --version $(IMAGE_VERSION) --align \
	$(IMAGE_ALIGN) -H $(IMAGE_HDR) $(BINFILE) $(SIGN_BINFILE)
	@$(COLOR_ECHO)
	@$(COLOR_ECHO) '${COLOR_PURPLE}Signed with key.pem for version $(IMAGE_VERSION)\
	.${COLOR_RESET}'
	@$(COLOR_ECHO)

.PHONY: flash-mcuboot

flash-mcuboot: HEXFILE = $(SIGN_BINFILE)

flash-mcuboot: mcuboot $(FLASHDEPS)
	$(FLASHER) $(FFLAGS)

endif # mcuboot
