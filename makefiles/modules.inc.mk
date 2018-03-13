ED = $(addprefix MODULE_,$(sort $(USEMODULE) $(USEPKG)))
EXTDEFINES = $(addprefix -D,$(shell echo '$(ED)' | tr 'a-z-' 'A-Z_'))

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
REALMODULES = $(filter-out $(PSEUDOMODULES), $(USEMODULE)) $(filter $(NO_PSEUDOMODULES), $(USEMODULE))
REALPKGS = $(filter-out $(PSEUDOMODULES), $(USEPKG)) $(filter $(NO_PSEUDOMODULES), $(USEPKG))
export BASELIBS += $(REALMODULES:%=$(BINDIR)/%.o) $(REALPKGS:%=$(BINDIR)/%.a)

CFLAGS += $(EXTDEFINES)

export USEMODULE
