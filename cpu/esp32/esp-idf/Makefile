MODULE=esp_idf

DIRS += driver
DIRS += esp32
DIRS += heap
DIRS += soc
DIRS += spi_flash

ifneq (,$(filter esp_net_wifi,$(USEMODULE)))
    DIRS += nvs_flash
    DIRS += wpa_supplicant
    INCLUDES += -I$(SDK_DIR)/components/smartconfig_ack/include
endif

ifneq (,$(filter esp_net_espeth,$(USEMODULE)))
    DIRS += ethernet
endif

include $(RIOTBASE)/Makefile.base
