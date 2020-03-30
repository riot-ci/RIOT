# Check if all required FEATURES are provided

FEATURES_OPTIONAL_ONLY := $(sort $(filter-out $(FEATURES_REQUIRED),$(FEATURES_OPTIONAL)))
FEATURES_OPTIONAL_USED := $(sort $(filter $(FEATURES_PROVIDED),$(FEATURES_OPTIONAL_ONLY)))
# Optional features that will not be used because they are not provided
FEATURES_OPTIONAL_MISSING := $(sort $(filter-out $(FEATURES_PROVIDED),$(FEATURES_OPTIONAL_ONLY)))

# Features that are used without taking "one out of" dependencies into account
FEATURES_USED_SO_FAR := $(sort $(FEATURES_REQUIRED) $(FEATURES_OPTIONAL_USED))

# Features that are provided and not blacklisted
FEATURES_USABLE := $(filter-out $(FEATURES_BLACKLIST),$(FEATURES_PROVIDED))

# Additionally required features due to the "one out of" dependencies
FEATURES_REQUIRED_ONE_OUT_OF := $(foreach item,$(FEATURES_REQUIRED_ANY),$(word 1,$(filter $(subst |, ,$(item)),$(FEATURES_USED_SO_FAR) $(FEATURES_USABLE)) $(item)))

# Features that are required by the application but not provided by the BSP
# Having features missing may case the build to fail.
FEATURES_MISSING := $(sort $(filter-out $(FEATURES_PROVIDED),$(FEATURES_REQUIRED) $(FEATURES_REQUIRED_ONE_OUT_OF)))

# Features that are used for an application
FEATURES_USED := $(sort $(FEATURES_REQUIRED) $(FEATURES_REQUIRED_ONE_OUT_OF) $(FEATURES_OPTIONAL_USED))

# Used features that conflict when used together
FEATURES_CONFLICTING := $(sort $(foreach conflict,$(FEATURES_CONFLICT),$(call _features_conflicting,$(conflict))))

# Return conflicting features from the conflict string feature1:feature2
#   $1: feature1:feature2
#   Return the list of conflicting features
_features_conflicting = $(if $(call _features_used_conflicting,$(subst :, ,$1)),$(subst :, ,$1))
# Check if all features from the list are used
#   $1: list of features that conflict together
#   Return non empty on error
_features_used_conflicting = $(filter $(words $1),$(words $(filter $(FEATURES_USED),$1)))

# Features that are used by the application but blacklisted by the BSP.
# Having blacklisted features may cause the build to fail.
FEATURES_USED_BLACKLISTED := $(sort $(filter $(FEATURES_USED), $(FEATURES_BLACKLIST)))
