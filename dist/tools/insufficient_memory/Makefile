-include $(DIR)/Makefile.ci

define create_Makefile.ci
	@echo "BOARD_INSUFFICIENT_MEMORY := \\" > $(1)
	@for b in $(sort $(BOARD_INSUFFICIENT_MEMORY)); do echo "    $$b \\" >> $(1); done
	@echo "    #" >> $(1)
endef

.PHONY: Makefile.ci
Makefile.ci: BOARD_INSUFFICIENT_MEMORY += $(BOARD)
ifeq ($(BOARD_INSUFFICIENT_MEMORY),)
Makefile.ci:
	@echo "skipping empty Makefile.ci"
else
Makefile.ci:
	$(call create_Makefile.ci, $(DIR)/Makefile.ci)
endif
