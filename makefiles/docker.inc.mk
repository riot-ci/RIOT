export DOCKER_IMAGE ?= riot/riotbuild:latest
export DOCKER_BUILD_ROOT ?= /data/riotbuild
DOCKER_RIOTBASE ?= $(DOCKER_BUILD_ROOT)/riotbase
export DOCKER_FLAGS ?= --rm
# List of Docker-enabled make goals
export DOCKER_MAKECMDGOALS_POSSIBLE = \
  all \
  buildtest \
  scan-build \
  scan-build-analyze \
  tests-% \
  #
export DOCKER_MAKECMDGOALS = $(filter $(DOCKER_MAKECMDGOALS_POSSIBLE),$(MAKECMDGOALS))

# Docker creates the files .dockerinit and .dockerenv in the root directory of
# the container, we check for the files to determine if we are inside a container.
ifneq (,$(wildcard /.dockerinit /.dockerenv))
  export INSIDE_DOCKER := 1
else
  export INSIDE_DOCKER := 0
endif

# Default target for building inside a Docker container if nothing was given
export DOCKER_MAKECMDGOALS ?= all
# List of all exported environment variables that shall be passed on to the
# Docker container, they will only be passed if they are set from the
# environment, not if they are only default Makefile values.
export DOCKER_ENV_VARS = \
  APPDIR \
  AR \
  ARFLAGS \
  AS \
  ASFLAGS \
  BINDIR \
  BINDIRBASE \
  BOARD \
  BUILDRELPATH \
  BUILDTEST_MCU_GROUP \
  BUILDTEST_VERBOSE \
  CC \
  CFLAGS \
  CPPMIX \
  CXX \
  CXXEXFLAGS \
  CXXUWFLAGS \
  ELFFILE \
  HEXFILE \
  LINK \
  LINKFLAGPREFIX \
  LINKFLAGS \
  LTO \
  OBJCOPY \
  OFLAGS \
  PREFIX \
  QUIET \
  WERROR \
  RIOT_VERSION \
  SCANBUILD_ARGS \
  SCANBUILD_OUTPUTDIR \
  SIZE \
  TOOLCHAIN \
  UNDEF \
  #

# Find which variables were set using the command line or the environment and
# pass those to Docker.
# DOCKER_ENVIRONMENT_CMDLINE must be immediately assigned (:=) or otherwise some
# of the environment variables will be overwritten by Makefile.include and their
# origin is changed to "file"
DOCKER_ENVIRONMENT_CMDLINE := $(foreach varname,$(DOCKER_ENV_VARS), \
  $(if $(filter environment command,$(origin $(varname))), \
  -e '$(varname)=$(subst ','\'',$($(varname)))', \
  ))
DOCKER_ENVIRONMENT_CMDLINE := $(strip $(DOCKER_ENVIRONMENT_CMDLINE))
# The variables set on the command line will also be passed on the command line
# in Docker
DOCKER_OVERRIDE_CMDLINE := $(foreach varname,$(DOCKER_ENV_VARS), \
    $(if $(filter command,$(origin $(varname))), \
    '$(varname)=$($(varname))', \
    ))
DOCKER_OVERRIDE_CMDLINE := $(strip $(DOCKER_OVERRIDE_CMDLINE))

# Overwrite if you want to use `docker` with sudo
DOCKER ?= docker


# # # # # # # # # # # # # # # #
# Directory mapping functions #
# # # # # # # # # # # # # # # #

# terminating '/' in patsubst is important to match $1 == $(RIOTBASE)
define dir_is_outside_riotbase
$(filter $1/,$(patsubst $(RIOTBASE)/%,%,$1/))
endef

# Mapping of directores inside docker
#
# $1 = directories (can be a list)
# $2 = docker remap base directory (defaults to DOCKER_BUILD_ROOT)
# $3 = mapname (defaults to $(notdir $d))
#
# For each directory:
#  * if inside, returns  $(DOCKER_RIOTBASE)/<relative_path_in_riotbase>
#  * if outside, returns <docker remapbase>/<mapname>
#
# From env:
#  * RIOTBASE
#  * DOCKER_RIOTBASE
#  * DOCKER_BUILD_ROOT
#
path_in_docker = $(foreach d,$1,$(strip $(call _dir_path_in_docker,$d,$2,$3)))
define _dir_path_in_docker
      $(if $(call dir_is_outside_riotbase,$1),\
        $(if $2,$2,$(DOCKER_BUILD_ROOT))/$(if $3,$3,$(notdir $1)),\
        $(patsubst %/,%,$(patsubst $(RIOTBASE)/%,$(DOCKER_RIOTBASE)/%,$1/)))
endef


DOCKER_APPDIR = $(DOCKER_BUILD_ROOT)/riotproject/$(BUILDRELPATH)


# This will execute `make $(DOCKER_MAKECMDGOALS)` inside a Docker container.
# We do not push the regular $(MAKECMDGOALS) to the container's make command in
# order to only perform building inside the container and defer executing any
# extra commands such as flashing or debugging until after leaving the
# container.
# The `flash`, `term`, `debugserver` etc. targets usually require access to
# hardware which may not be reachable from inside the container.
..in-docker-container:
	@$(COLOR_ECHO) '$(COLOR_GREEN)Launching build container using image "$(DOCKER_IMAGE)".$(COLOR_RESET)'
	$(DOCKER) run $(DOCKER_FLAGS) -t -u "$$(id -u)" \
	    -v '$(RIOTBASE):$(DOCKER_RIOTBASE)' \
	    -v '$(RIOTCPU):$(DOCKER_BUILD_ROOT)/riotcpu' \
	    -v '$(RIOTBOARD):$(DOCKER_BUILD_ROOT)/riotboard' \
	    -v '$(RIOTMAKE):$(DOCKER_BUILD_ROOT)/riotmake' \
	    -v '$(RIOTPROJECT):$(DOCKER_BUILD_ROOT)/riotproject' \
	    -v /etc/localtime:/etc/localtime:ro \
	    -e 'RIOTBASE=$(DOCKER_RIOTBASE)' \
	    -e 'CCACHE_BASEDIR=$(DOCKER_RIOTBASE)' \
	    -e 'RIOTCPU=$(DOCKER_BUILD_ROOT)/riotcpu' \
	    -e 'RIOTBOARD=$(DOCKER_BUILD_ROOT)/riotboard' \
	    -e 'RIOTMAKE=$(DOCKER_BUILD_ROOT)/riotmake' \
	    -e 'RIOTPROJECT=$(DOCKER_BUILD_ROOT)/riotproject' \
	    $(DOCKER_ENVIRONMENT_CMDLINE) \
	    -w '$(DOCKER_APPDIR)' \
	    '$(DOCKER_IMAGE)' make $(DOCKER_MAKECMDGOALS) $(DOCKER_OVERRIDE_CMDLINE)
