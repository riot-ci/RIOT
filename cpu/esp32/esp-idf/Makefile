MODULE=esp_idf

DIRS += driver
DIRS += esp32
DIRS += heap
DIRS += soc
DIRS += spi_flash

ifeq ($(ENABLE_WIFI), 1)
DIRS += nvs_flash
endif

include $(RIOTBASE)/Makefile.base
