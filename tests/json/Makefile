include ../Makefile.tests_common

# blacklist due to error: size of array is too large
BOARD_BLACKLIST +=  arduino-duemilanove \
                    arduino-mega2560 \
                    arduino-uno \
                    chronos \
                    jiminy-mega256rfr2 \
                    mega-xplained \
                    msb-430 \
                    msb-430h \
                    telosb \
                    waspmote-pro \
                    wsn430-v1_3b \
                    wsn430-v1_4 \
                    z1 \
                    #

BOARD_INSUFFICIENT_MEMORY :=    airfy-beacon \
                                arduino-mkr1000 \
                                arduino-mkrfox1200 \
                                arduino-mkrzero \
                                arduino-zero \
                                b-l072z-lrwan1 \
                                blackpill \
                                bluepill \
                                calliope-mini \
                                cc2650-launchpad \
                                cc2650stk \
                                ek-lm4f120xl \
                                esp8266-esp-12x \
                                esp8266-olimex-mod \
                                esp8266-sparkfun-thing \
                                feather-m0 \
                                ikea-tradfri \
                                limifrog-v1 \
                                lobaro-lorabox \
                                maple-mini \
                                microbit \
                                nrf51dongle \
                                nrf6310 \
                                nucleo-f030r8 \
                                nucleo-f031k6 \
                                nucleo-f042k6 \
                                nucleo-f070rb \
                                nucleo-f072rb \
                                nucleo-f091rc \
                                nucleo-f103rb \
                                nucleo-f302r8 \
                                nucleo-f303k8 \
                                nucleo-f334r8 \
                                nucleo-f410rb \
                                nucleo-l031k6 \
                                nucleo-l053r8 \
                                nucleo-l073rz \
                                nucleo-l432kc \
                                nucleo-l433rc \
                                nz32-sc151 \
                                opencm904 \
                                pba-d-01-kw2x \
                                samd21-xpro \
                                saml21-xpro \
                                samr21-xpro \
                                samr30-xpro \
                                sensebox_samd21 \
                                slstk3401a \
                                sltb001a \
                                slwstk6220a \
                                sodaq-autonomo \
                                sodaq-explorer \
                                sodaq-one \
                                sodaq-sara-aff \
                                spark-core \
                                stk3600 \
                                stm32f0discovery \
                                stm32f3discovery \
                                teensy31 \
                                yunjia-nrf51822 \
                                #

USEMODULE += embunit
USEMODULE += json

TEST_ON_CI_WHITELIST += native

include $(RIOTBASE)/Makefile.include
