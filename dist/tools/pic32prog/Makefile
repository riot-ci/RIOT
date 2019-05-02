PKG_NAME     = pic32prog
PKG_URL      = https://github.com/sergev/pic32prog
PKG_VERSION  = f5d27d6c9506bc0d1f2053002e2aadad1ac424cd
PKG_LICENSE  = GPL-2
PKG_BUILDDIR = bin

# Building it requires some dependencies, on ubuntu:
#
#     sudo apt-get install libusb-dev libusb-1.0-0-dev libudev-dev

all: git-download
	@echo "[INFO] compiling pic32prog from source now"
	@env -i PATH=$(PATH) TERM=$(TERM) make -C $(PKG_BUILDDIR)
	@mv $(PKG_BUILDDIR)/pic32prog pic32prog

distclean::
	@rm -f pic32prog

include $(RIOTBASE)/pkg/pkg.mk
