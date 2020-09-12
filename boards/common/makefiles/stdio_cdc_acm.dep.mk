ifeq (,$(filter-out stdio_cdc_acm,$(filter stdio_% slipdev_stdio,$(USEMODULE))))
  # Use stdio_cdc_acm only if no other stdio is requested explicitly.
  FEATURES_REQUIRED += stdio_cdc_acm
  USEMODULE += stdio_cdc_acm
endif
