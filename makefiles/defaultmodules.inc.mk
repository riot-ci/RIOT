DEFAULT_MODULE += board cpu core core_init core_msg core_panic sys

DEFAULT_MODULE += auto_init
DEFAULT_MODULE += auto_init_actuators_sensors_default

# Initialize all used peripherals by default
DEFAULT_MODULE += periph_init
