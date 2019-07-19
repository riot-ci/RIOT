#
# makes any file in BLOBS available via '#include "blob/<filename>.h"
#
# Usage:
#
# Add this to an application or module Makefile:
#
#     BLOBS += foo.ext
#
# Then include in C file or header:
#
#    #include "blob/foo.ext.h"
#

# use "blobs/blob" so the headers can be included as "blob/foo.h", but
# we don't have to add $(BINDIR)/$(MODULE) to the include path.
BLOB_HDR_DIR ?= $(BINDIR)/$(MODULE)/blobs/blob
BLOB_H := $(BLOBS:%=$(BLOB_HDR_DIR)/%.h)

ifneq (, $(BLOB_H))
	# add $(BINDIR)/$(MODULE)/blobs to include path
	CFLAGS += -I$(dir $(BLOB_HDR_DIR))
endif

$(BLOB_HDR_DIR):
	@mkdir -p $@

$(BLOB_H): $(BLOB_HDR_DIR)
$(BLOB_H): $(BLOB_HDR_DIR)/%.h: % $(BLOBS)
	cd $(dir $<); xxd -i $(notdir $<) | sed 's/^unsigned/const unsigned/g'> $@

# make C and C++ objects of this module depend on generated headers, so they
# get re-build on changes to the blob files.
$(OBJC) $(OBJCXX): $(BLOB_H)
