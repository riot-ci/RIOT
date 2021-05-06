# This file contains a collection of targets defined by tools shipped with RIOT.
# The reason for keeping those targets separated from their tool Makfiles is
# that these targets have to be defined after the default build targets
# (as `all`), so `all` will always be the first target defined and thereby the
# default target when `make` is called without any further argument.

.PHONY: mosquitto_rsmb

# target for building the bossac binary
$(RIOTTOOLS)/bossa-$(BOSSA_VERSION)/bossac:
	@echo "[INFO] bossac $(BOSSA_VERSION) binary not found - building it from source"
	@$(MAKE) -C $(RIOTTOOLS)/bossa-$(BOSSA_VERSION)
	@echo "[INFO] bossac $(BOSSA_VERSION) binary successfully built!"

$(RIOTTOOLS)/pic32prog/pic32prog: $(RIOTTOOLS)/pic32prog/Makefile
	@echo "[INFO] $(@F) binary not found - building it from source now"
	$(MAKE) -C $(@D)
	@echo "[INFO] $(@F) binary successfully built!"

$(RIOTTOOLS)/cc2538-bsl/cc2538-bsl.py: $(RIOTTOOLS)/cc2538-bsl/Makefile
	@echo "[INFO] cc2538-bsl.py not found - fetching it from GitHub now"
	@CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/cc2538-bsl
	@echo "[INFO] cc2538-bsl.py successfully fetched!"

$(PKGDIRBASE)/cosy/cosy.py: $(RIOTTOOLS)/cosy/Makefile
	@echo "[INFO] cosy.py not found - fetching it from GitHub now"
	@CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/cosy
	@echo "[INFO] cosy.py successfully fetched!"

$(RIOTTOOLS)/edbg/edbg: $(RIOTTOOLS)/edbg/Makefile
	@echo "[INFO] edbg binary not found - building it from source now"
	CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/edbg
	@echo "[INFO] edbg binary successfully built!"

$(RIOTTOOLS)/mosquitto_rsmb/mosquitto_rsmb:
	@echo "[INFO] rsmb binary not found - building it from source now"
	@$(MAKE) -C $(RIOTTOOLS)/mosquitto_rsmb
	@echo "[INFO] rsmb binary successfully built!"

mosquitto_rsmb: $(RIOTTOOLS)/mosquitto_rsmb/mosquitto_rsmb
	@$(MAKE) -C $(RIOTTOOLS)/mosquitto_rsmb run

$(RIOTTOOLS)/setsid/setsid: $(RIOTTOOLS)/setsid/Makefile
	@echo "[INFO] setsid binary not found - building it from source now"
	@$(MAKE) -C $(RIOTTOOLS)/setsid
	@echo "[INFO] setsid binary successfully built!"

$(RIOTTOOLS)/suit/suit-manifest-generator/bin/suit-tool: $(RIOTTOOLS)/suit/Makefile
	@echo "[INFO] suit-manifest-generator not found - fetching it now"
	@$(MAKE) -C $(RIOTTOOLS)/suit
	@echo "[INFO] suit-manifest-generator successfully fetched!"

$(RIOTTOOLS)/flatc/flatc: $(RIOTTOOLS)/flatc/Makefile
	@echo "[INFO] flatc binary not found - building it from source now"
	$(MAKE) -C $(RIOTTOOLS)/flatc
	@echo "[INFO] flatc binary successfully built!"

$(RIOTTOOLS)/lpc2k_pgm/bin/lpc2k_pgm: $(RIOTTOOLS)/lpc2k_pgm/Makefile
	@echo "[INFO] lpc2k_pgm binary not found - building it from source now"
	@$(MAKE) -C $(RIOTTOOLS)/lpc2k_pgm
	@echo "[INFO] lpc2k_pgm binary successfully built!"

$(RIOTTOOLS)/uf2/uf2conv.py: $(RIOTTOOLS)/uf2/Makefile
	@echo "[INFO] uf2conv.py not found - fetching it from GitHub now"
	CC= CFLAGS= $(MAKE) -C $(RIOTTOOLS)/uf2
	@echo "[INFO] uf2conv.py successfully fetched!"
