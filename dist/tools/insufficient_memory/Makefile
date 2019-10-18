-include $(DIR)/Makefile.ci

# workaround for buildsystem_sanity_check/check.sh
_BOARD_INSUFFICIENT_MEMORY := $(BOARD_INSUFFICIENT_MEMORY)

define create_Makefile.ci
	@echo "BOARD_INSUFFICIENT_MEMORY := \\" > $(1)
	@for b in $(sort $(_BOARD_INSUFFICIENT_MEMORY)); do echo "    $$b \\" >> $(1); done
	@echo "    #" >> $(1)
endef

_BOARD_INSUFFICIENT_MEMORY += $(BOARD)

.PHONY: Makefile.ci
ifeq ($(_BOARD_INSUFFICIENT_MEMORY),)
Makefile.ci:
	@echo "skipping empty Makefile.ci"
else
Makefile.ci:
	$(call create_Makefile.ci, $(DIR)/Makefile.ci)
endif
