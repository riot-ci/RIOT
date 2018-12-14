DEFAULT_MODULE += board cpu core core_msg sys

DEFAULT_MODULE += auto_init
# this adds periph_init by default, other periph drivers
# (e.g. i2c or spi) are only added when used
DEFAULT_MODULE += periph_common
