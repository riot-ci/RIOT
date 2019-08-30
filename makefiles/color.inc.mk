# Set colored output control sequences if the terminal supports it and if
# not disabled by the user

COLOR_GREEN  :=
COLOR_RED    :=
COLOR_YELLOW :=
COLOR_PURPLE :=
COLOR_RESET  :=
COLOR_ECHO   := /bin/echo

ifeq ($(CC_NOCOLOR),)
  available_colors:=$(shell tput colors 2> /dev/null)
  ifeq ($(available_colors),)
    CC_NOCOLOR = 1
  else ifeq ($(available_colors),-1)
    CC_NOCOLOR = 1
  else ifeq ($(available_colors),0)
    CC_NOCOLOR = 1
  else
    CC_NOCOLOR = 0
  endif
endif

ifeq ($(CC_NOCOLOR),0)
  COLOR_GREEN  := $(ANSI_GREEN)
  COLOR_RED    := $(ANSI_RED)
  COLOR_YELLOW := $(ANSI_YELLOW)
  COLOR_PURPLE := $(ANSI_PURPLE)
  COLOR_RESET  := $(ANSI_RESET)
  ifeq ($(OS),Darwin)
    COLOR_ECHO   := echo -e
    SHELL=bash
  else
    COLOR_ECHO   := /bin/echo -e
  endif
endif
