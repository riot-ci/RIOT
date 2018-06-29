## Introduction

**RIOT-Xtensa-ESP** is a bare metal implementation of **RIOT-OS** for **ESP32** SOCs which supports most features of RIOT-OS. The peripheral SPI and I2C interfaces allow to connect all external hardware modules supported by RIOT-OS, such as sensors and actuators. SPI interface can also be used to connect external IEEE802.15.4 modules to integrate ESP32 boards into a GNRC network.

Although the port does not use the official **ESP-IDF** (Espresso IoT Development Framework) SDK, it must be installed for compilation. The reason is that the port uses most of the **ESP32 SOC definitions** provided by the ESP-IDF header files. In addition, it needs the hardware abstraction library (libhal), and the **ESP32 WiFi stack binary** libraries which are part of the ESP-IDF SDK.

## Features

The RIOT-OS for ESP32 SoCs supports the following features at moment:

- I2C interfaces
- SPI interfaces
- UART interfaces
- CPU ID access
- RTC module
- ADC and DAC channels
- PWM channels
- SPI RAM
- SPI Flash Drive (MTD with SPIFFS and VFS)
- Hardware number generator
- Hardware timer devices
- ESP-NOW netdev interface
- ESP Ethernet MAC (EMAC) netdev interface

## Limitations

The implementation of RIOT-OS for ESP32 SOCs has the following limitations at the moment:

- Only **one core** (the PRO CPU) is used because RIOT does not support running multiple threads simultaneously.
- **Bluetooth** cannot be used at the moment.
- **Flash encryption** is not yet supported.

## Toolchain

Following software components are required for compilation:

- **Xtensa GCC** compiler suite for ESP32
- **ESP-IDF** SDK which includes all ESP32 SOC definitions, the hardware abstraction library ```libhal.a```, and the flash programmer tool ```esptool.py```

The installation of the toolchain is described in detail below. Alternatively, a preconfigured **docker** image could be used, see section _Using docker Image_ below.

### Installation of Xtensa GCC compiler suite

Xtensa GCC compiler for ESP32 can be downloaded and installed as precompiled binary archive from Espressif's web page.

```
mkdir -p $HOME/esp
cd $HOME/esp
wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
tar -xzf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
rm xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
```
Once the compiler is installed you can add the binary directory to your ```PATH``` variable.
```
export PATH=$PATH:$HOME/esp/xtensa-esp32-elf/bin
```

### Installation of ESP-IDF (Espressif IoT Development Framework)

ESP-IDF, the official SDK from Espressif, can be dowloaded and installed as GIT repository.

```
cd $HOME/esp
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
git checkout f198339
cd components/esp32/lib
git checkout 534a9b1

```
Please take care to checkout right branches which were used for the port. Newer versions might also work but were not tested.

Since we only use a few header files, ESP-IDF does not need to be compiled in any way. To use the installed ESP-IDF, just set the variable ```SDK_DIR``` accordingly.
```
export SDK_DIR=$HOME/esp/esp-idf
```

### Using docker Image

The easiest way to compile RIOT for ESP32 is to use the preconfigured docker image ```schorcht/riotdocker_esp32```. This docker image has installed all requiered components in the ```/opt/esp``` directory. To use the docker image, just change to the RIOT's root directory and use the following command
```
docker run -i -t --privileged -v /dev:/dev -u $UID -v $(pwd):/data/riotbuild schorcht/riotbuild_esp32
```
to pull and run the docker container. Inside the docker container, RIOT's root directory is the home directory of user ```riotbuild```. That is, you can start with compilation directly. The environment contains all settings required for the compilation.

### Usage

To compile an application for an ESP32 board, change to RIOT's root directory and execute the make command, e.g.,
```
make flash BOARD=esp32-generic -C tests/shell [Compile Options]
```
where the ```BOARD``` variable specifies the generic ESP32 board definition and option ```-C``` the directory of application.

#### Compile Options

The make process can be configured by a number of command-line variables for the make command, e.g., ```ENABLE_SPIFFS=1``` to activate a SPIFFS drive in built-in flash memory.

Following table shows all possible command-line options in alphabetical order.

Option | Values | Default | Meaning
:------|:-------|:--------|:-------
BOARD_APP_CONF | file name | empty | Specify an application specific board configuration file located in application source directory, see section _Application Specific Board Configuration_.
CONFIGS | string | empty | Specify application specific board configurations, see section _Application Specific Board Configuration_.
ENABLE_GDB | 0, 1 | 0 | Enable the compilation with debug information for debugging with QEMU (```QEMU=1```), see section _QEMU Mode and GDB_ below.
ENABLE_ESP_NOW | 0, 1 | 0 | Enable the built-in WiFi module with ESP_NOW protocol to use it as ```netdev```, see section _ESP_NOW Interface_.
ENABLE_ESP_ETH | 0, 1 | 0 | Enable the Ethernat interface ```EMAC``` to use it as ```netdev```, see section _Ethernet Interface_ below.
ENABLE_HW_COUNTER | 0, 1 | 0 | Enable the timer implementation that uses counters instead of hardware timer modules, see section _Timer Implementations_ below.
ENABLE_MRF24J40 | 0, 1 | 0 | Enable MRF24J40 module to use IEEE 802.15.4 radio modules based on MRF24J40.
ENABLE_SPIFFS | 0, 1 | 0 | Enable SPIFFS file system, see section _SPIFFS Drive_ below.
ENABLE_SPI_RAM | 0, 1 | 0 | Enable SPI RAM, see section _SPI RAM_ below.
ENABLE_SW_I2C | 0, 1 | 0 | Enable the I2C software implementation, see section _I2C Implementations_ below.
PORT | /dev/ttyUSBx | /dev/ttyUSB0 | Set the port for flashing the firmware.
QEMU | 0, 1 | 0 | Use QEMU mode and generate an image for QEMU, see _QEMU Mode and GDB_ below.
USE_ESP_IDF_NEWLIB | 0, 1 | 1 | Use the ```newlib``` C library from ESP-IDF (```USE_ESP_IDF_NEWLIB=1```), see below.
USE_ESP_IDF_HEAP | 0, 1 | 0 | Use the complex heap implementation from ESP-IDF (```USE_ESP_IDF_HEAP=1```), see below.

## Application Specific Board Configuration

The board configuration in ```board.h``` usually defines a default configuration which GPIOs are used for which purpose. However, because GPIOs can be used for different purposes, it is often necessary to change some of the default configurations for individual applications. For example, while many PWM channels are needed in one application, another application does not need PWM channels, but many ADC channels. Board configurations in ```board.h``` are usually only a compromise between these different requirements. There are two ways to give the application the ability to change some of the predefined configurations:

- ```CONFIGS``` make variable
- application specific board configuration file

Using the ```CONFIGS``` make variable at command line, single board definitions can be overridden, e.g., a flag which indicates that a LCD display is connected and which is normally not set:
```
CONFIGS='-DESP_LCD_PLUGGED_IN=1'
```

When a number of board definitions have to be overridden, this approach is impractical. In that case, an application specific board configuration file located in application source directory can be used. For example, if a file ```esp32_wrover_kit_conf.h``` in application source directory would define some board configuration for the ESP32 WROVER Kit board, it could be specified as ```BOARD_APP_CONF``` make variable at command line:
```
BOARD_APP_CONF=esp32_wrover_kit_conf.h
```

## ESP-IDF newlib and Heap Implementation

ESP-IDF provides two components that can be used with RIOT:

- ```newlib``` C library
- complex heap implementation

While the version of the ```newlib``` C library shipped with the precompiled Xtensa GCC compiler suite requires POSIX thread library ```pthreads```, the ```newlib``` version from the ESP IDF SDK does not need it. By default, the ```newlib``` version from the ESP-IDF SDK is used ```(USE_ESP_IDF_NEWLIB=1)```. To use the ```newlib``` C library from the Xtensa GCC compiler suite, just define ```USE_ESP_IDF_NEWLIB=0``` as make variable at the command line.

The ESP-IDF SDK provides a complex heap implementation that supports multiple heap segments in different memory areas such as DRAM, IRAM, and PSRAM. Whenever you want to use these memory areas as heap, you have to use the heap implementation from the ESP-IDF SDK (```USE_ESP_IDF_HEAP=1```).

**Please note:**
If the ```newlib``` version from ESP-IDF SDK is used (```USE_ESP_IDF_NEWLIB=1```), also the heap implementation from ESP-IDF SDK has to be used. In that case it is enabled by default (```USE_ESP_IDF_HEAP=1```). Otherwise, it is disabled by default (```USE_ESP_IDF_HEAP=0```) and RIOT's simple heap implementation is used which support only one heap segment in DRAM.

**Please note:**
To use SPI RAM (```ENABLE_SPI_RAM=1```), the heap implementation from ESP-IDF SDK has also to be used. In that case it is also enabled by default (```USE_ESP_IDF_HEAP=1```).

## SPIFFS Drive

If SPIFFS module is enabled (```ENABLE_SPIFFS=1```), the implemented MTD device ```mtd0``` uses the built-in SPI flash memory together with the modules SPIFFS and VFS to realize a persistent file system.

For this purpose, flash memory is formatted as SPIFFS the first time the system is started. The start address is determined from the flash partition table and starts at a multiple of 0x100000 (1 MByte). Another option is to configure the start address with define ```CONFIG_SPI_FLASH_DRIVE_START``` in ```$(RIOTCPU)/$(CPU)/include/sdkconfig.h```. If this define exists and is not 0, the configured start address is checked for conformance and is then used.

Please refer file ```$(RIOTBASE)/tests/unittests/test-spiffs/tests-spiffs.c``` for more information on how to use SPIFFS and VFS together with a MTD device ```mtd0``` alias ```MTD_0```.

## SPI RAM modules

Some ESP32 modules, e.g., the WROVER module, integrate external RAM that is connected via the FSPI interface. To activate and use this external RAM, set the make variable ```ENABLE_SPI_RAM=1``` at command line. By default, SPI RAM is not activated and cannot be used.

**Please note**:
ESP32 uses four data lines to access the external SPI RAM, called quad output flash mode (QOUT). Therefore, GPIO9 and GPIO10 are used as SPI data lines and are not available for other purposes.

**Please note**
Enabling SPI RAM for modules that don't have SPI RAM may lead to boot problems for some modules. For others is simply throws an error message.

## I2C Implementations

The ESP32 has two built-in I2C hardware interfaces that support I2C bus speed up to 400 kbps (```I2C_SPEED_FAST```). The default pin configuration of I2C interfaces can be changed by the application, see section _Application Specific Board Configuration_.

Device          |Signal|Pin     |Symbol         |Remarks
:---------------|:-----|:-------|:--------------|:----------------
```I2C_DEV(0)```| SCL  | GPIO22 |```I2C0_SCL``` | cannot be changed
```I2C_DEV(0)```| SDA  | GPIO21 |```I2C0_SDA``` | cannot be changed
```I2C_DEV(1)```| SCL  | -      |```I2C1_SCL``` | not declared
```I2C_DEV(1)```| SDA  | -      |```I2C1_SDA``` | not declared

If the according I2C interfaces are not to be used, ```I2C0_NOT_AVAILABLE``` and/or ```I2C1_NOT_AVAILABLE``` have to be declared instead. Most boards definitions do not declare I2C_DEV(1).

Beside the I2C hardware implementation, a I2C bit-banging protocol software implementation  can be used. This implementation allow I2C bus speeds up to 1 Mbps (```I2C_SPEED_FAST_PLUS```). It can be activated by defining ```ENABLE_SW_I2C=1``` as make variable at the command line. However, since it uses busy waiting, you should always prefer the hardware implementation when possible which is used by default.

## SPI Interfaces

ESP32 integrates four SPI controllers:

- controller SPI0 is reserved for accessing flash memory
- controller SPI1 realizes interface **```FSPI```** which shares its signals with SPI0
- controller SPI2 realizes interface **```HSPI```** that can be used for peripherals
- controller SPI3 realizes interface **```VSPI```** that can be used for peripherals

Since SPI0 controller is used to access flash and other external memories, at most three interfaces can be used:

- **```VSPI```**: realized by controller SPI3 and mapped to ```SPI_DEV(0)```
- **```HSPI```**: realized by controller SPI2 and mapped to ```SPI_DEV(1)```
- **```FSPI```**: realized by controller SPI1 and mapped to ```SPI_DEV(2)```.

All SPI interface could be used in quad SPI mode, but RIOT's low level device driver doesn't support it.

Since the interface ```SPI_DEV(2)``` shares its bus signals with the controller that implements the flash memory interface, we use the name FSPI for this interface. In the technical reference, this interface is misleadingly simply referred to as SPI.

**Please note**:
Since the FSPI interface ```SPI_DEV(2)``` shares its bus signals with flash memory interface and optionally other external memories, you can only use ```SPI_DEV(2)``` to attach external memory with same SPI mode and same bus speed but with a different CS. Using ```SPI_DEV(2)``` for anything else can disturb flash memory access which causes a number of problems. If not really necessary, you should not use this interface.

Even though the following default pin configuration of SPI interfaces is used on most boards, it can vary from board to board. The pin configuration of ```VSPI``` and ```HSPI``` interface can be changed by the application, see section _Application Specific Board Configuration_.

Interface |Device           |Signal|Pin     |Symbol         |Remarks
:---------|:----------------|:-----|:-------|:--------------|:----------------
```VSPI```|```SPI_DEV(0)``` | SCK  | GPIO18 |```SPI0_SCK``` | optional, can be overridden
```VSPI```|```SPI_DEV(0)``` | MISO | GPIO19 |```SPI0_MISO```| optional, can be overridden
```VSPI```|```SPI_DEV(0)``` | MOSI | GPIO23 |```SPI0_MOSI```| optional, can be overridden
```VSPI```|```SPI_DEV(0)``` | CS0  | GPIO18 |```SPI0_CS0``` | optional, can be overridden
```HSPI```|```SPI_DEV(1)``` | SCK  | GPIO14 |```SPI0_SCK``` | optional, can be overridden
```HSPI```|```SPI_DEV(1)``` | MISO | GPIO12 |```SPI0_MISO```| optional, can be overridden
```HSPI```|```SPI_DEV(1)``` | MOSI | GPIO13 |```SPI0_MOSI```| optional, can be overridden
```HSPI```|```SPI_DEV(1)``` | CS0  | GPIO15 |```SPI0_CS0``` | optional, can be overridden
```FSPI```|```SPI_DEV(2)``` | SCK  | GPIO14 |```SPI0_SCK``` | mandatory, cannot be overridden
```FSPI```|```SPI_DEV(2)``` | MISO | GPIO12 |```SPI0_MISO```| mandatory, cannot be overridden
```FSPI```|```SPI_DEV(2)``` | MOSI | GPIO13 |```SPI0_MOSI```| mandatory, cannot be overridden
```FSPI```|```SPI_DEV(2)``` | CS0  | GPIO15 |```SPI0_CS0``` | mandatory, cannot be overridden

## UART Interfaces

ESP32 supports up to three UART devices UART_DEV(0) pin configuration is fix. All ESP32 boards use it as standard configuration. The pin configuration of ```UART_DEV(1)``` and ```UART_DEV(2)``` can be changed.

The following default pin configuration of UART interfaces can be overridden by the application, see section _Application Specific Board Configuration_.

Device            |Signal|Pin     |Symbol         |Remarks
:-----------------|:-----|:-------|:--------------|:----------------
```UART_DEV(0)``` | TxD  | GPIO1  |```UART0_TXD```| cannot be changed
```UART_DEV(0)``` | RxD  | GPIO3  |```UART0_RXD```| cannot be changed
```UART_DEV(1)``` | TxD  | GPIO10 |```UART1_TXD```| optional, can be overridden
```UART_DEV(1)``` | RxD  | GPIO9  |```UART1_RXD```| optional, can be overridden
```UART_DEV(2)``` | TxD  | GPIO17 |```UART2_TXD```| optional, can be overridden
```UART_DEV(2)``` | RxD  | GPIO16 |```UART2_RXD```| optional, can be overridden

If the according UART interfaces are not to be used, ```UART1_NOT_AVAILABLE``` and/or ```UART2_NOT_AVAILABLE``` have to be declared instead. Most boards definitions do not declare UART_DEV(1) and UART_DEV(2).

## PWM Channels

ESP supports two types of PWM generators

- one LED PWM controller (LEDPWM) with 16 channels, and
- two high-speed Motor Control PWM controllers (MCPWM) with 6 channels each.

The PWM implementation uses the ESP32's high-speed MCPWM modules. Reason is that the LED PWM controller only supports resolutions of powers of two.

Using both Motor Control PWM controllers with 6 channels each, the maximum number of PWM
devices is 2 and the maximum total number of PWM channels is 12.

Which PWM device can drive which GPIOs is declared by the defines ```PWM0_GPIOS``` for ```PWM_DEV(0)``` and ```PWM1_GPIOS``` for ```PWM_DEV(1)```, respectively.

**Please note:**
As long as the respective PWM device is not initialized with the ```pwm_init``` function, these GPIOs can be used for other purposes. That is, it would be theoretically possible to declare all 12 PWM channels without using them.

The definition of ```PWM0_GPIOS``` and ```PWM1_GPIOS``` can be omitted or empty. In the latter case, they must at least contain the curly braces. The corresponding PWM device can not be used in this case.

Furthermore, ```PWM0_GPIOS_NOT_AVAILABLE``` and ```PWM1_GPIOS_NOT_AVAILABLE``` can be declared to indicate the the according device is not used.

Board definitions usually declare a number of GPIOs as PWM channels. This configuration can be changed by the application, see section _Application Specific Board Configuration_.

## ADC Channels

ESP32 integrates two 12-bit ADCs (ADC1 and ADC2) capable of measuring up to 18 analog signals in total. Most of these ADC channels are either connected to a number of intergrated sensors like a Hall sensors, touch sensors and a temperature sensor or can be connected with certain GPIOs. Integrated sensors are disabled in RIOT's implementation and are not accessible. Thus, up to 18 GPIOs, can be used as ADC inputs:

- ADC1 supports 8 channels: GPIOs 32-39
- ADC2 supports 10 channels: GPIOs 0, 2, 4, 12-15, 25-27

These GPIOs are realized by the RTC unit and are therefore also called RTC GPIOs or RTCIO GPIOs.

**Please note**: GPIO37 and GPIO38 are normally not broken out on ESP32 modules and are therefore not usable.

**```ADC_GPIOS```** declares which GPIOs can be used as ADC channels by an application for a certain board. The order of the listed GPIOs determines the mapping between the RIOT's ADC lines and the GPIOs. The definition of ```ADC_NUMOF``` is derived automatically from ```ADC_GPIOS``` and must not be changed.

Board definitions usually declare a number of possible GPIOs as ADC channels. This configuration can be changed by the application, see section _Application Specific Board Configuration_.
```ADC_GPIOS``` is usually defined in ```board.h```.

**Please note:**
Declaration of ```ADC_GPIOS``` must not be empty. If no ADC pins are to be used, just omit the ```ADC_GPIOS``` declaration and declare ```ADC_GPIOS_NOT_AVAILABLE``` instead.

**Please note:**
As long as the GPIOs listed in ```ADC_GPIOS``` are not initialized as ADC channels with the ```adc_init``` function, they can be used for all other uses of GPIOs.

## DAC Channels

ESP32 supports 2 DAC lines at GPIO25 and GPIO26. These DACs have a width of 8 bits and produce voltages in the range from 0 V to 3.3 V (VDD_A). The 16 bits DAC values given as parameter of function *dac_set* are down-scaled to 8 bit.

**```DAC_GPIOS```** declares which GPIOs can be used as DAC channels by an application for a certain board. The order of the listed GPIOs determines the mapping between the RIOT's DAC lines and the GPIOs. The definition of ```DAC_NUMOF``` is derived automatically from ```DAC_GPIOS``` and must not be changed.

Board definitions usually declare one or two GPIOs as DAC channels. This configuration can be overridden by the application, see section _Application Specific Board Configuration_.
```DAC_GPIOS``` is usually defined in ```board.h```.

**Please note:**
Declaration of ```DAC_GPIOS``` must not be empty. If no DAC pins are to be used, just omit the ```DAC_GPIOS``` declaration and declare ```DAC_GPIOS_NOT_AVAILABLE``` instead.

**Please note:**
As long as the GPIOs listed in ```DAC_GPIOS``` are not initialized as DAC channels with the ```dac_init``` function, they can be used for all other uses of GPIOs.

## Timer Implementations

There are two different implementations for hardware timers.

- timer module implementation
- counter implementation

The hardware timer module provides 4 high-speed timers, where 1 timer is used for system time. The remaining 3 timers with one channel each can be used as timer devices with a clock rate of 1 MHz.

The hardware counter implementation uses CCOUNT/CCOMPARE registers to implement 2 timers devices with one channel each and a clock rate of 1 MHz.

By default, hardware timer module is used. To use the hardware counter implementation define ```ENABLE_HW_COUNTER=1``` for the make command.

## ESP-NOW Network Interface

With ESP-NOW (```ENABLE_ESP_NOW=1```) the ESP32 provides a connectionless communication technology, featuring short packet transmission. It applies the IEEE802.11 Action Vendor frame technology, along with the IE function developed bEspressif, and CCMP encryption technology, realizing a secure, connectionless communication solution.

The ```netdev``` driver uses ESP-NOW to provide a link layer interface to a meshed network of ESP32 nodes. In this network, each node can send short packets with up to 250 data bytes to all other nodes that are visible in its area.

The ESP32 nodes are used in the ESP-NOW COMBO role along with the integrated WiFi interface's SoftAP + station mode to advertise their SSID and become visible to other ESP32 nodes.

The SSID of an ESP32 node is the concatenation of the prefix ```RIOT_ESP_``` with the MAC address of its SoftAP WiFi interface. The driver periodically scans all visible ESP32 nodes. The period can be configured by driver parameters.

Using the driver parameters, the encrypted communication can be enabled or disabled. All nodes in a network must use either encrypted or unencrypted communication.

**Please note:**
If encrypted communication is used, a maximum of 6 nodes can communicate with each other, while in unencrypted mode, up to 20 nodes can communicate.

## Ethernet Network Interface

ESP32 provides an **Ethernet MAC layer module (EMAC)** according to the IEEE 802.3 standard which can be used together with an external physical layer module (PHY) to realize a 100/10 Mbps Ethernet interface. As PHY modules Microchip LAN8710/LAN8720 and Texas Instrument TLK110 are supported.

The ```netdev``` driver uses RIOTs standard Ethernet interface. It is enabled with ```ENABLE_ESP_ETH=1```.

**Please node**
The board has to have one of the supported PHY modules to be able to use the Ethernet MAC module.

## Flash Modes

Principally, **ESP32 modules** can be flashed with ```qio```, ```qout```, ```dio``` and ```dout```. Although using ```qio``` or ```qout``` increases the performance of SPI flash data transfers, these modes require the two additional GPIO's 9 and 10. That is, in these flash modes these GPIO's are not available. Therefore, the default flash mode is ```dout``` and cannot be changed.

For more information about flash mode, refer the documentation of [esptool.py](https://github.com/espressif/esptool/wiki/SPI-Flash-Modes).

## JTAG Debugging

For debugging, ESP32 provides a JTAG interface at GPIOs 12 ... 15.

ESP32 Pin     | ESP32 signal name JTAG Signal
:-------------|:-----------
CHIP_PU       | TRST_N
GPIO15 (MTDO) | TDO
GPIO12 (MTDI) | TDI
GPIO13 (MTCK) | TCK
GPIO14 (MTMS) | TMS
GND           | GND

This JTAG interface can be used together with OpenOCD and GDB to debug your software on instruction level. When you compile your software with debugging information (```ENABLE_GDB=1```) you can also debug on source code level as well.

**Please note:**
When debugging, the GPIOs used for the JTAG interface must not be used for anything else.

Detailed information on how to configure the JTAG interface of the ESP32 and to setup of OpenOCD and GDB can be found in section JTAG Debugging in the [ESP-IDF Programming Guide](https://esp-idf.readthedocs.io/en/latest/api-guides/jtag-debugging/index.html).

## QEMU Mode and GDB

When you execute command ```make flash``` with QEMU mode enabled (```QEMU=1```), instead of loading the image to the target hardware, a binary image called ```esp32flash.bin``` is created in the target directory. Furthermore, two ROM binary files ```rom.bin``` and ```rom1.bin``` are copied to the target directory. This files file can then be used together with QEMU to debug the code in GDB.

The binary image can be compiled with debugging information (```ENABLE_GDB=1```) or optimized without debugging information (```ENABLE_GDB=0```). The latter one is the default. The version with debugging information can be debugged in source code while the optimized version can only be debugged in assembler mode.

To use QEMU, you have to install QEMU for Xtensa with ESP32 machine implementation as following.

```
cd $HOME/src
git clone git://github.com/Ebiroll/qemu_esp32
cp qemu_esp32/bin/xtensa-esp32-elf-gdb $HOME/esp/xtensa-esp32-elf/bin/xtensa-esp32-elf-gdb.qemu
rm -rf qemu_esp32

git clone git://github.com/Ebiroll/qemu-xtensa-esp32
cd qemu-xtensa-esp32
./configure --disable-werror --prefix=$HOME/esp/qemu-esp32 --target-list=xtensa-softmmu,xtensaeb-softmmu
make install
cd ..; rm -rf qemu-xtensa-esp32 # optional
```

Once the compilation has been finished, QEMU for Xtensa with ESP32 machine implementation should be available in ```$HOME/esp/qemu-esp32``` and you can change to your application target directory to start it in one terminal window , e.g.,

```
cd $HOME/src/RIOT-Xtensa-ESP/tests/shell/bin/esp32-generic
$HOME/esp/qemu-esp32/bin/qemu-system-xtensa -d guest_errors,unimp -cpu esp32 -M esp32 -m 4M -S -s > io.txt
```
where ```$HOME/src/RIOT-Xtensa-ESP``` is the root directory of RIOT and ```tests/shell``` is the application.

**Please note:**
QEMU starts always the files ```esp32flash.bin```, ```rom.bin``` and ```rom1.bin``` in local directory. Therefore, please take care to ensure that you are in right target directory before you start QEMU.

In second terminal window, you can then start GDB and connect to the emulation for the example.
```
xtensa-esp32-elf-gdb.qemu $HOME/src/RIOT-Xtensa-ESP/tests/shell/bin/esp32-generic/tests_shell.elf
```
To start debugging, you have to connect to QEMU with command:
```
(gdb) target remote :1234
```

**Please note**:
QEMU for Xtensa ESP32 does not support interrupts. That is, once your application uses interrupts, e.g., timers, the application cannot be debugged using QEMU together with GDB.
