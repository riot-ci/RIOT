apt install -y cmake curl git build-essential vim python-setuptools \
    python-argparse python3-pip python-pip mosquitto-clients socat g++-multilib \
    net-tools pcregrep libpcre3 gcc-avr binutils-avr avr-libc \
    avrdude doxygen cppcheck valgrind coccinelle \
    gcc-msp430 mspdebug unzip pkg-config jimsh libtool \
    #

DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true apt install -y tshark

# IoT-LAB CLI tools
pip3 install iotlabwscli iotlabsshcli iotlabcli

# Python tools
pip3 install aiocoap pyocd paho-mqtt pyserial flake8 tox \
            pyasn1 ecdsa pexpect pycrypto ed25519 cbor cryptography \
            scapy codespell protobuf jupyterlab \
            #

# OpenOCD
apt install -y build-essential libftdi-dev libhidapi-dev \
        libusb-1.0-0-dev libudev-dev autoconf libsqlite3-dev \
        libpopt-dev libxml2-dev ruby libtool pkg-config

git clone https://github.com/ntfreak/openocd openocd && \
    cd openocd && \
    git checkout 09ac9ab135ed35c846bcec4f7d468c3656852f26 && \
    ./bootstrap && ./configure && \
    make && \
    make install && \
    cd .. && rm -rf openocd

# JLink
JLINK_PKG_DEB="JLink_Linux_V644g_x86_64.deb"
wget -nv http://demo-fit.saclay.inria.fr/vms/utils/${JLINK_PKG_DEB} && \
dpkg -i ${JLINK_PKG_DEB} && \
rm -f ${JLINK_PKG_DEB}
