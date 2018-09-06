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
  RIOT_CI_BUILD \
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
# Handles relative directories
define dir_is_outside_riotbase
$(filter $(abspath $1)/,$(patsubst $(RIOTBASE)/%,%,$(abspath $1)/))
endef

# Mapping of directores inside docker
#
# $1 = directories (can be a list of relative files)
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
        $(if $2,$2,$(DOCKER_BUILD_ROOT))/$(if $3,$3,$(notdir $(abspath $1))),\
        $(patsubst %/,%,$(patsubst $(RIOTBASE)/%,$(DOCKER_RIOTBASE)/%,$(abspath $1)/)))
endef

# Volume mapping and environment arguments
#
# Docker arguments for directories:
#
# * volume mapping for each directory not in RIOT
# * remap environment variable directories to the docker ones
#
# $1 = variable name (content can be a list)
# $2 = docker remap base directory (defaults to DOCKER_BUILD_ROOT)
# $3 = mapname (defaults to $(notdir $d))
docker_volume_and_env = $(strip $(call _docker_volume_and_env,$1,$2,$3))
define _docker_volume_and_env
  $(call docker_volumes_mapping,$($1),$2,$3)
  $(call docker_environment_mapping,$1,$2,$3)
endef
docker_volumes_mapping = $(foreach d,$1,$(call _volume_mapping,$d,$2,$3))
_volume_mapping = $(if $1,$(if $(call dir_is_outside_riotbase,$1), -v '$(abspath $1):$(call path_in_docker,$1,$2,$3)'))
docker_cmdline_mapping = $(if $($1),'$1=$(call path_in_docker,$($1),$2,$3)')
docker_environment_mapping = $(addprefix -e,$(call docker_cmdline_mapping,$1,$2,$3))


# Application directory relative to either riotbase or riotproject
DOCKER_RIOTPROJECT = $(call path_in_docker,$(RIOTPROJECT),,riotproject)
DOCKER_APPDIR = $(DOCKER_RIOTPROJECT)/$(BUILDRELPATH)


# Directory mapping in docker and directories environment variable configuration
DOCKER_VOLUMES_AND_ENV += -v /etc/localtime:/etc/localtime:ro
DOCKER_VOLUMES_AND_ENV += -v '$(RIOTBASE):$(DOCKER_RIOTBASE)'
DOCKER_VOLUMES_AND_ENV += -e 'RIOTBASE=$(DOCKER_RIOTBASE)'
DOCKER_VOLUMES_AND_ENV += -e 'CCACHE_BASEDIR=$(DOCKER_RIOTBASE)'

DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,RIOTPROJECT,,riotproject)
DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,RIOTCPU,,riotcpu)
DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,RIOTBOARD,,riotboard)
DOCKER_VOLUMES_AND_ENV += $(call docker_volume_and_env,RIOTMAKE,,riotmake)

# Remap external module directories. Not handled if they have common dirnames
ifneq ($(words $(sort $(notdir $(EXTERNAL_MODULE_DIRS)))),$(words $(sort $(EXTERNAL_MODULE_DIRS))))
  $(warnings EXTERNAL_MODULE_DIRS: $(EXTERNAL_MODULE_DIRS))
  $(error Mapping EXTERNAL_MODULE_DIRS in docker is not supported for directories with the same name)
endif
# EXTERNAL_MODULE_DIRS should ignore the 'Makefile' configuration, so set
# variables using command line arguments
DOCKER_VOLUMES_AND_ENV += $(call docker_volumes_mapping,$(EXTERNAL_MODULE_DIRS),$(DOCKER_BUILD_ROOT)/external,)
DOCKER_OVERRIDE_CMDLINE += $(call docker_cmdline_mapping,EXTERNAL_MODULE_DIRS,$(DOCKER_BUILD_ROOT)/external,)


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
	    $(DOCKER_VOLUMES_AND_ENV) \
	    $(DOCKER_ENVIRONMENT_CMDLINE) \
	    -w '$(DOCKER_APPDIR)' \
	    '$(DOCKER_IMAGE)' make $(DOCKER_MAKECMDGOALS) $(DOCKER_OVERRIDE_CMDLINE)
