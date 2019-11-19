# Makefile for the NRF24L01+ radio driver
MODULE = nrf24l01p

# enable submodules
SUBMODULES := 1

# Always build base module
SRC := gnrc_netif_nrf24l01p.c nrf24l01p_communication.c nrf24l01p_diagnostics.c \
	nrf24l01p_internal.c nrf24l01p_isr.c nrf24l01p_lookup_tables.c \
	nrf24l01p_netdev.c nrf24l01p_registers.c nrf24l01p_states.c \
	nrf24l01p.c

include $(RIOTBASE)/Makefile.base
