# Make only version of string functions
#
# This replaces the pattern of using ':= $(shell echo $(var) | tr 'a-z-' 'A-Z_)'
# On local tests the make version was ~100 times faster than the shell one

lowercase = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))
uppercase = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$1))))))))))))))))))))))))))
uppercase_and_underscore = $(call uppercase,$(subst -,_,$1))

# Padds $2 number to $1 digits
_pad_number = $(shell printf '%0$1d' $2)

# Gets mayor, minor, patch from 'mayor.minor.patch', e.g.: 4.2.1 by index
#   $1: index
#   $2: version
_version = $(word $1, $(subst ., ,$2))

# Returns padded version 'mayor.minor.patch' to 3 digits
# eg: 4.2.1 -> 004.002.001
#   $1: version
_padded_version = $(subst $(space),.,$(foreach var,1 2 3,$(call _pad_number,3,$(call _version,$(var),$1))))

# Checks if versions $1 is greater than version $2
#   $1: version to check, '.' separated version 'mayor.minor.patch'
#   $2: minimum version, '.' separated version 'mayor.minor.patch'
#   Return 1 if $1 is greater than $2, nothing otherwise
_is_lower = $(if $(filter $1,$(firstword $(sort $1 $2))),,1)
version_is_greater = $(call _is_lower,$(call _padded_version,$1),\
                        $(call _padded_version,$2))
