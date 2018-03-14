ED = $(addprefix MODULE_,$(sort $(USEMODULE) $(USEPKG)))
EXTDEFINES = $(addprefix -D,$(shell echo '$(ED)' | tr 'a-z-' 'A-Z_'))

# filter "pseudomodules" from "real modules", but not "no_pseudomodules"
# filter out any duplicate module names
REALMODULES = $(shell echo $(filter-out $(PSEUDOMODULES) $(USEARCHIVE), $(USEMODULE)) \
  $(filter $(NO_PSEUDOMODULES), $(USEMODULE)) | tr ' ' '\n' | awk '!a[$$0]++')
REALPKGS = $(filter-out $(PSEUDOMODULES) $(USEARCHIVE), $(USEPKG)) $(filter $(NO_PSEUDOMODULES), $(USEPKG))
REALARCHIVES = $(filter $(USEARCHIVE),$(USEMODULE) $(USEPKG))
export BASELIBS += $(REALMODULES:%=$(BINDIR)/%.o) $(REALPKGS:%=$(BINDIR)/%.o) $(REALARCHIVES:%=$(BINDIR)/%.a)

CFLAGS += $(EXTDEFINES)

export USEMODULE
