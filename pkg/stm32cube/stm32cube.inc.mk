PKG_NAME=stm32cube

include $(RIOTMAKE)/utils/strings.mk
STM32_VARIANT = $(call uppercase,$(CPU_FAM))

PKG_URL=https://github.com/STMicroelectronics/STM32Cube$(STM32_VARIANT)
PKG_VERSION_F0=568c7255f77258c12ac876745f53ce76a682259f  # v1.11.0
PKG_VERSION_F1=441b2cbdc25aa50437a59c4bffe22b88e78942c9  # v1.8.0
PKG_VERSION_F2=42fc8bf966c04ef814bb0620dcd3e036e038b4a2  # v1.9.0
PKG_VERSION_F3=8fa3aadf0255818f0ca72ba6a5a6731ef8c585fb  # v1.11.0
PKG_VERSION_F4=a86ecaa2fb63029596ba7dabadab2d9c2c139560  # v1.25.0
PKG_VERSION_F7=79acbf8ec060d3ec751f2eaba6ee050269995357  # v1.16.0
PKG_VERSION_L0=65f3157a89766c2f89442115c49d29df73485eb9  # v1.11.2
PKG_VERSION_L1=910c24274e58b6c179894706d16cbc5162ced586  # v1.9.0
PKG_VERSION_L4=285336eeffa12f9cee3a1784a6b60744bd43c0d9  # v1.15.1
PKG_VERSION_WB=6f39fbe2a5edac79006e4c4dc527c1573713dcfc  # v1.7.0

PKG_VERSION=$(PKG_VERSION_$(STM32_VARIANT))
PKG_LICENSE=BSD-3-Clause

PKG_BUILDDIR = $(CURDIR)/cache/$(CPU_FAM)
