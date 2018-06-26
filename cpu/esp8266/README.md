# RIOT-OS port for ESP8266 and ESP8285

There are two implementations that can be used:

- the **SDK version** which is realized on top of an SDK (esp-open-sdk or ESP8266_NONOS_SDK) and
- the **non-SDK version** which is realized without the SDK.

The non-SDK version produces a much smaller code size than the SDK version and is more efficient in excecution because it does not need to run additional SDK functions to keep the SDK system alive.

The **non-SDK** version is probably the **best choice if you do not need the built-in WiFi module**, for example, when you plan to connect the MCU to an IEEE 802.15.4 radio module for communication.

By default, the non-SDK version is compiled. To compile the SDK-version, add option ```SDK=1``` to the make command, e.g.,

    make flash BOARD=esp8266-esp-12x -C test/shell SDK=1 ...

For more information, see section **Compile Options**.

## Tool Chain

Following software components are required:

- **esp-open-sdk** which includes the Xtensa GCC compiler toolchain, the hardware abstraction library for Xtensa LX106, and the flash programmer tool ```esptool.py```
- **newlib-c** library for xtensa (esp-open-rtos version)
- **SDK (optional)**, either as part of ```esp-open-sdk``` or the ```ESP8266_NONOS_SDK```

### Installation of esp-open-sdk

esp-open-sdk is directly installed inside its source directory. Therefore, change directly to the target directory of the toolchain to build it.

```
cd /path/to/esp
git clone --recursive https://github.com/pfalcon/esp-open-sdk.git
cd esp-open-sdk
export ESP_OPEN_SDK_DIR=$PWD
```

If you plan to use the SDK-version of the RIOT port and to use the SDK as part of esp-open-sdk, simply build its standalone version.

```
make STANDALONE=y
```

If you only plan to use the non-SDK version of the RIOT port or if you want to use one of Espressif's original SDKs, it is enough to build the toolchain.

```
make toolchain esptool libhal STANDALONE=n
```

Once compilation has been finished, the toolchain is available in **```$PWD/xtensa-lx106-elf/bin```**. To use it, set the **```PATH```** variable accordingly.

```
export PATH=$ESP_OPEN_SDK_DIR/xtensa-lx106-elf/bin:$PATH
```

If you have compiled the standalone version of esp-open-sdk and you plan to use this SDK version, set additionally the **```SDK_DIR```** variable.

```
export SDK_DIR=$ESP_OPEN_SDK_DIR/sdk
```

### Installation of newlib-c

First, set the target directory for the installation.

```
export NEWLIB_DIR=/path/to/esp/newlib-xtensa
```

Please take care, to use the newlib-c version that was modified for esp-open-rtos since it includes ```stdatomic.h```.

```
cd /my/source/dir
git clone https://github.com/ourairquality/newlib.git
```

Once you have cloned the GIT repository, build and install it with following commands.
```
cd newlib
./configure --prefix=$NEWLIB_DIR --with-newlib --enable-multilib --disable-newlib-io-c99-formats --enable-newlib-supplied-syscalls --enable-target-optspace --program-transform-name="s&^&xtensa-lx106-elf-&" --disable-option-checking --with-target-subdir=xtensa-lx106-elf --target=xtensa-lx106-elf --enable-newlib-nano-formatted-io --enable-newlib-reent-small
make
make install
```

### Installtion of Espressif original SDK (optional)

If you plan to use the SDK version of the RIOT port and if you want to use one of Espressif's original SDKs, you have to install it.

First, download the **ESP8266_NONOS_SDK** version 2.1.0 from the [Espressif web site](https://github.com/espressif/ESP8266_NONOS_SDK/releases/tag/v2.1.0). Propably other version might also work. However, RIOT port is tested with bersion 2.1.0.

Once you have downloaded it, you can install it with following commands.

```
cd /path/to/esp
tar xvfz /downloads/ESP8266_NONOS_SDK-2.1.0.tar.gz
```

To use the installed SDK, set variable **```SDK_DIR```** accordingly.

```
export SDK_DIR=/path/to/esp/ESP8266_NONOS_SDK-2.1.0
```

## Usage

Once you have installed all required components, you should have the following directories.

```
/path/to/esp/esp-open-sdk
/path/to/esp/newlib-xtensa
/path/to/esp/ESP8266_NONOS_SDK-2.1.0 (optional)
```

To use the toolchain and optionally the SDK, please check that your environment variables are set correctly to

```
export PATH=/path/to/esp/esp-open-sdk/xtensa-lx106-elf/bin:$PATH
export NEWLIB_DIR=/path/to/esp/newlib-xtensa
```
and optionally
```
export SDK_DIR=/path/to/esp/esp-open-sdk/sdk
```
or

```
export SDK_DIR=/path/to/esp/ESP8266_NONOS_SDK-2.1.0
```

## Compile Options

The compilation process can be configured with various command-line options for the make command. Following table shows these command-line options.

Option | Values | Default | Meaning
-------|--------|---------|--------
ENABLE_GDB | 0, 1 | 0 | Enable compilation with debug information for debugging with QEMU (```QEMU=1```), see section _QEMU Mode and GDB_ below
ENABLE_SPI | 0, 1 | 0 | Enable the HSPI interface using the GPIOs with prefix SPI defined in ```periph_cpu.conf```, see section _SPI Interface_ below.
ENABLE_SPIFFS | 0, 1 | 0 | Enable or disable the SPIFFS file system, see section _SPIFFS Module_ below.
ENABLE_SW_TIMER | 0, 1 | 0 | Enable software timer implementation, only available in conjunction with ```SDK=1```, see section _Timer Implementations_ below.
FLASH_MODE | dout, dio, qout, qio | dout | Set the flash mode, please take care with your module, see section _Flash Modes_ below.
PORT | /dev/ttyUSBx | /dev/ttyUSB0 | Set the port for flashing the firmware.
QEMU | 0, 1 | 0 | Use QEMU mode and generate an image for QEMU, see _QEMU Mode and GDB_ below.
SDK | 0, 1 | 0 | Compile the SDK version (```SDK=1```) or non-SDK version (```SDK=0```).
```SDK=1```, see section _SDK Task Handling_ below.

## SPI Interface

ESP8266 port provides exactly one SPI interface ```SPI_DEV(0)``` that uses additional GPIO's as following:

Signal | Pin
-------|--------
MISO   | GPIO12
MOSI   | GPIO13
SCK    | GPIO14

When SPI interface is enabled, these GPIOs cannot be used for any other purpose. Therefore, the SPI interface has to be enabled explicitly during compilation with (```ENABLE_SPI=1```).

As CS signal GPIOs 0, 2, 4, 5 or 15 can be used. In flash modes ```dio``` and ```dout``` (see section _Flash Modes_), GPIO's 9 and 10 can also be as CS signal.

## SPIFFS Module

If SPIFFS module is enabled (```ENABLE_SPIFFS=1```), implemented MTD device ```mtd0``` for the build-in SPI flash memory is used together with modules SPIFFS and VFS to realize a file system.

For this purpose, flash memory is formatted as SPIFFS starting at the address ```0x80000``` (512 kbyte) on first boot. All sectors up to the last 5 sectors of the flash memory are then used for the file system. With a fixed sector size of 4096 bytes, the top address of the SPIFF is ```flash Size - 5 * 4096```, e.g., ```0xfb000``` for a flash memory of 1 MByte. The size of the SPIFF then results from ```flash size - 5 * 4096 - 512 kByte```.

Please refer file ```$(RIOTBASE)/tests/unittests/test-spiffs/tests-spiffs.c``` for more information on how to use SPIFFS and VFS together with a MTD device ```mtd0``` alias ```MTD_0```.

## Timer Implementations

Per default, the **hardware timer implementation** is used. In this implementation there is avaibable one timer with only one channel.

If you use the SDK version of the RIOT port (```SDK = 1```), you can activate the **Software-Timer** (```ENABLE_SW_TIMER=1```), which implements one timer with 10 channels. The software timer uses SDK software timers to implement various timer channels. Although these SDK timers usually have a precision of a few microseconds, they can deviate up to 500 microseconds. So if you need a timer with high accuracy, you'll need to use the hardware timer with just one timer channel.

## Flash Modes

**ESP8266 modules** can be flashed with ```qio```, ```qout```, ```dio``` and ```dout```. Using ```qio``` or ```qout``` increases the performance of SPI flash data transfers but uses two additional GPIO's 9 and 10. That is, in this flash modes these GPIO's are not available.

**ESP8285 modules** have to be always flashed with ```dout```.

For more information about flash mode, refer the documentation of [esptool.py](https://github.com/espressif/esptool/wiki/SPI-Flash-Modes).

## QEMU Mode and GDB

When QEMU mode is enabled (```QEMU=1```), instead of loading the image to the target hardware, a binary image ```$(ELFFILE).bin``` is created in the target directory. This binary image file can be used together with QEMU to debug the code in GDB.

The binary image can be compiled with degugging information (```ENABLE_GDB=1```) or optimized without debugging information (```ENABLE_GDB=0```). The latter one is the default. The version with debugging information can be debugged in source code while the optimized version can only be debugged in assembler mode.

To use QEMU, you have to install QEMU for Xtensa with ESP8266 machine implementation as following.

```
cd /my/source/dir
git clone https://github.com/gschorcht/qemu-xtensa
cd qemu-xtensa/
git checkout xtensa-esp8266
export QEMU=/path/to/esp/qemu
./configure --prefix=$QEMU --target-list=xtensa-softmmu --disable-werror
make
make install
```

Once the compilation has been finished, QEMU for Xtensa with ESP8266 machine implementation should be available in ```/path/to/esp/qemu/bin``` and you can start it with

```
$QEMU/bin/qemu-system-xtensa -M esp8266 -nographic -serial stdio -monitor none -s -S -kernel /path/to/the/target/image.elf.bin
```

where ```/path/to/the/target/image.elf.bin``` is the path to the binary image as generated by the ```make``` command as ```$(ELFFILE).bin```. After that you can start GDB in another terminal window using command:

```
xtensa-lx106-elf-gdb
```

If you have compiled your binary image with debugging information, you can load the ELF file in gdb with:

```
(gdb) file /path/to/the/target/image.elf
```

To start debugging, you have to connect to QEMU with command:
```
(gdb) target remote :1234
```

## I2C Interface

Since ESP8266 does not support I2C in hardware, the I2C interface is realized as bit-banging protocol in software. The maximum usable bus speed is therefore ```I2C_SPEED_FAST_PLUS```. The maximum number of busses that can be defined is 3 (```I2C_DEV(0) ... ```I2C_DEV(2)```.

Number of I2C busses and GPIO pins used as signals for theses busses have to be defined in the board specific peripheral configuration in ```board.h```. In the following example only one I2C with the two signals ```I2C_SDA_0``` and ```I2C_SCL_0``` for bus 0 are defined.

```
#define I2C_NUM 1
#define I2C_SDA_0   GPIO4
#define I2C_SCL_0   GPIO5
```
A configuration of two I2C busses could look like the following.

```
#define I2C_NUM 2
#define I2C_SDA_0   GPIO4
#define I2C_SCL_0   GPIO5
#define I2C_SDA_1   GPIO2
#define I2C_SCL_1   GPIO14
```

## PWM Channels

Up to 8 GPIOs can be defined as PWM channels to generate PWM output signals. By default, GPIOs 2, 4 and 5 are defined as PWM channels. As long as these channels are not started with function ```pwm_set```, they can be used as normal GPIOs for other functions.

To defined other GPIOs as PWM channels, just overwrite the definition of ```PWM_CHANNEL_GPIOS``` in your ```board.h``` file.

```
#define PWM_CHANNEL_GPIOS { GPIO12, GPIO13, GPIO14, GPIO15 }
```

The maximum number of PWM clock cycles per second is 100.000 (period of 10 us). That is, the product of PWM signal frequency and PWM sinal resolution must not be greater than 100.000. Otherwise, the PWM signal frequency is reduced.

## Other Peripheral Features

The ESP8266 port of RIOT also supports

- one ADC channel with a resolution of 10 bit, please refer your hardware manual,
- one hardware number generator,
- one UART interface
- a CPU-ID function,
- a power management.

RTC is not yet implemented.
