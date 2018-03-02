#!/bin/sh

get_cmd_version() {
    if [ -z "$1" ]; then
        return
    fi

    local cmd="$1"
    if command -v "$cmd" 2>&1 >/dev/null; then
        ver=$("$cmd" --version 2> /dev/null | head -n 1)
        if [ -z "$ver" ]; then
            ver="error"
        fi
    else
        ver="missing"
    fi

    printf "%s" "$ver"
}

get_define() {
    local cc="$1"
    local line=
    if command -v "$cc" 2>&1 >/dev/null; then
        line=$(echo "$3" | "$cc" -x c -include "$2" -E -o - - 2>&1 | sed -e '/^[   ]*#/d' -e '/^[  ]*$/d')
    fi
    if [ -z "$line" ]; then
        line=missing
    fi
    printf "%s" "$line"
}

newlib_version() {
    if [ -z "$1" ]; then
        cc=gcc
    else
        cc=$1-gcc
    fi
    printf "%s" "$(get_define "$cc" newlib.h _NEWLIB_VERSION)"
}

avr_libc_version() {
    if [ -z "$1" ]; then
        cc=gcc
    else
        cc=$1-gcc
    fi
    printf "%s (%s)" "$(get_define "$cc" avr/version.h __AVR_LIBC_VERSION_STRING__)" "$(get_define "$cc" avr/version.h __AVR_LIBC_DATE_STRING__)"
}

printf "%s\n" "Installed compiler toolchains "
printf "%s\n" "-----------------------------"
printf "%21s: %s\n" "native gcc" "$(get_cmd_version gcc)"
for p in msp430 avr arm-none-eabi mips-mti-elf; do
    printf "%21s: %s\n" "$p-gcc" "$(get_cmd_version ${p}-gcc)"
done
printf "%21s: %s\n" "clang" "$(get_cmd_version clang)"
printf "\n"
printf "%s\n" "Installed compiler libs"
printf "%s\n" "-----------------------"
for p in arm-none-eabi mips-mti-elf; do
    printf "%21s: %s\n" "$p-newlib" "$(newlib_version "$p")"
done
for p in avr; do
    printf "%21s: %s\n" "$p-libc" "$(avr_libc_version "$p")"
done
printf "\n"
printf "%s\n" "Installed development tools"
printf "%s\n" "---------------------------"
for c in cmake cppcheck git; do
    printf "%21s: %s\n" "$c" "$(get_cmd_version $c)"
done
printf "%21s: %s\n" "coccinelle" "$(get_cmd_version spatch)"

exit 0
