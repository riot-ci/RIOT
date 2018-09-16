PROTOC ?= protoc
PROTOC_GEN_NANOPB ?= protoc-gen-nanopb

PROTOBUF_FILES ?= $(wildcard *.proto)
GENSRC   += $(PROTOBUF_FILES:%.proto=$(BINDIR)/$(MODULE)/%.pb.c)
GENOBJC  := $(GENSRC:%.c=%.o)

ifneq (, $(PROTOBUF_FILES))
  INCLUDES += -I$(BINDIR)/$(MODULE)
endif

$(SRC): $(GENSRC)

$(GENSRC): $(PROTOBUF_FILES)
	$(Q)D=$(BINDIR)/$(MODULE) && \
	  mkdir -p "$$D" && \
		cd $(CURDIR) && \
		for protofile in $(PROTOBUF_FILES); do \
			protoc --plugin=protoc-gen-nanopb=$(PROTOC_GEN_NANOPB) \
				--nanopb_out="$$D" \
				$^ \
				; done
