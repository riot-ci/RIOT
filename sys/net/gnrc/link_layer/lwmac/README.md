LwMAC implementation
====================

# Radio duty cycling

Lw-MAC adopts the radio duty-cycle scheme to conserve power. Namely, in each cycle period (MAC superframe), a node device wakes up for a short period of time (called listen period or wakeup period) for receiving possible incoming packets from other devices. Outside the listen period, the node device turns off its radio to conserve power.

# Phase-lock scheme

Lw-MAC adopts the phase-lock scheme to further reduce power consumption. Each node device in Lw-MAC will try to record/track its Tx-neighbor's wakeup phase. This is called phase-lock. After phase-locking, the sender node will (likely) spend less preamble packets (also called WR packet, i.e., wakeup-request, in Lw-MAC) for initiating a hand-shaking procedure for transmitting a data packet, compared to the first time it talks to the receiver.

# Burst transmission

Lw-MAC adopts pending-bit technique to enhance its throughput. Namely, in case of having multi packets for the receiver, a sender uses the pending-bit flag embedded in the MAC header to instruct this situation, and the buffered packets will be transmitted in a continuous sequence, back to back, to the receiver in one shot.

# Auto wakeup extension

Lw-MAC adopts auto wakeup extension scheme based on timeout (like T-MAC). In short, when a packet is successfully received at the receiver side, the receiver will reset the wakeup timeout to extend its wakeup period for receiving more potential incoming packets. This is to be compatible with the pending-bit technique to allow the receiver to absorb more packets when needed, thus boosts the throughput.

# Simple retransmission scheme

Lw-MAC adopts a simple retransmission scheme to enhace link reliability. The data packet will only be dropped in case the retransmission counter gets larger than LWMAC_MAX_DATA_TX_RETRIES.

# Automatic phase backoff scheme

Lw-MAC adopts an automatic phase backoff scheme to reduce WR (Preamble) collision probability. In multi-hop scenarios, let's say, nodes A <---B <----C (which is common in multi-hop data collection networks), in which B has packets for A, and C has packets for B. In case A and B's wakeup phases are too close (overlapping). Then, especially in high traffic conditions, B and C may initiate transmissions at the same time (B sends to A, and C sends to B), a link of either will be definitely interfered, leading to collisions and link throughput reduction. To this end, by using the automatic phase backoff scheme, if a sender finds its receiver's phase is too close to its own phase, it will run a backoff scheme to randomly reselect a new wakeup phase for itself.
