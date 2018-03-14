ED = $(addprefix MODULE_,$(sort $(USEMODULE) $(USEPKG)))
EXTDEFINES = $(addprefix -D,$(shell echo '$(ED)' | tr 'a-z-' 'A-Z_'))

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
# filter out any duplicate module names
REALMODULES = $(shell echo $(filter-out $(PSEUDOMODULES), $(USEMODULE)) \
  $(filter $(NO_PSEUDOMODULES), $(USEMODULE)) | tr ' ' '\n' | awk '!a[$$0]++')
# USEPKG packages are by default using static archives (.a) for linking, but
# pkgs listed in USEMODULE are using object files (.o), so we filter out USEMODULE here
REALPKGS = $(filter-out $(PSEUDOMODULES) $(USEMODULE), $(USEPKG)) \
  $(filter $(NO_PSEUDOMODULES), $(filter-out $(USEMODULE), $(USEPKG)))
export BASELIBS += $(REALMODULES:%=$(BINDIR)/%.o) $(REALPKGS:%=$(BINDIR)/%.a)

CFLAGS += $(EXTDEFINES)

export USEMODULE
