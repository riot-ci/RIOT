# generate .py.h header files of .py files
PY_HEADER_DIR ?= $(BINDIR)/$(MODULE)py_h
PY = $(wildcard *.py)
PY_H := $(PY:%.py=$(PY_HEADER_DIR)/%.py.h)

ifneq (, $(PY_H))
	CFLAGS += -I$(PY_HEADER_DIR)
endif

$(PY_HEADER_DIR):
	@mkdir -p $@

$(PY_H): $(PY_HEADER_DIR)
$(PY_H): $(PY_HEADER_DIR)/%.py.h: %.py $(PY)
	cd $(dir $<); xxd -i $(notdir $<) > $@

# make C and C++ objects of this module depend on generated headers, so they
# get re-build on changes to the python files.
$(OBJC) $(OBJCXX): $(PY_H)
