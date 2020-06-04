DEFAULT_MODULE += board cpu
DEFAULT_MODULE += core core_idle_thread core_init core_msg core_panic
DEFAULT_MODULE += sys

DEFAULT_MODULE += auto_init

# Initialize all used peripherals by default
DEFAULT_MODULE += periph_init
