# Set the tests default configuration
APP_MTU ?= 5000
APP_BUF_CHUNKSIZE ?= 250    # must be full divider of APP_MTU
APP_BUF_NUM ?= 3
APP_NODENAME ?= \"nimble_l2cap_test_server\"
APP_CID ?= 0x0235

# Apply configuration values
CFLAGS += -DAPP_MTU=$(APP_MTU)
CFLAGS += -DAPP_BUF_CHUNKSIZE=$(APP_BUF_CHUNKSIZE)
CFLAGS += -DAPP_BUF_NUM=$(APP_BUF_NUM)
CFLAGS += -DAPP_NODENAME=$(APP_NODENAME)
CFLAGS += -DAPP_CID=$(APP_CID)

# configure NimBLE
USEPKG += nimble
CFLAGS += -DMYNEWT_VAL_BLE_L2CAP_COC_MAX_NUM=1
CFLAGS += -DMYNEWT_VAL_BLE_L2CAP_COC_MPS=250
CFLAGS += -DMYNEWT_VAL_BLE_MAX_CONNECTIONS=1
CFLAGS += -DMYNEWT_VAL_MSYS_1_BLOCK_COUNT=23
CFLAGS += -DMYNEWT_VAL_MSYS_1_BLOCK_SIZE=292
CFLAGS += -DMYNEWT_VAL_BLE_LL_CFG_FEAT_DATA_LEN_EXT=1

INCLUDES += -I$(RIOTBASE)/tests/nimble_l2cap_server/include
