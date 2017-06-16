#!/bin/sh

DLCACHE=${DLCACHE:-~/.dlcache}

mkdir -p "$DLCACHE"

if [ "$(uname)" = Darwin ]; then
    _locked() {
        local lockfile="$1"
        shift

        while ! shlock -p $$ -f $lockfile; do
            sleep 0.2
        done

        $*

        rm $lockfile
    }
else
    _locked() {
        local lockfile="$1"
        shift

        (
        flock -w 600 9 || exit 1
        $*
        ) 9>"$lockfile"
    }
fi

if [ "$(uname)" = Darwin ]; then
    MD5="md5 -r"
else
    MD5=md5sum
fi

md5() {
    local file="$1"
    local md5="$2"

    local file_md5=$(${MD5} "$file" | cut -d\  -f1)

    test "$md5" = "$file_md5"
}

downloader() {
    if [ -n "$(command -v wget)" ]; then
        wget -nv "$1" -O $2
    elif [ -n "$(command -v curl)" ]; then
        curl -L $1 -o $2
    else
        echo "$0: neither wget nor curl available!"
        return 1
    fi
}

download() {
    local url="$1"
    local _md5="$2"
    local basename_url=$(basename ${url})
    local target="${3:-${basename_url}}"

    [ -f "$target" ] && {
        # if our target file exists, check it's md5.
        md5 "$target" "$_md5" && {
            echo "$0: target exists, md5 matches."
            exit 0
        }
    }

    local filename="$(basename $url)"
    [ -f "$DLCACHE/$filename" ] && {
        # if the file exists in cache, check it's md5 and possibly remove it.
        if md5 "$DLCACHE/$filename" "$_md5"; then
            echo "$0: getting \"$url\" from cache"
        else
            echo "$0: \"$DLCACHE/$filename\" has wrong checksum, re-downloading"
            rm "$DLCACHE/$filename"
        fi
    }

    [ ! -f "$DLCACHE/$filename" ] && {
        echo "$0: downloading \"$url\""
        downloader "$url" "$DLCACHE/$filename" || {
            echo "$0: error downloading $url to $DLCACHE/$filename!"
            exit 1
        }
    }

    md5 "$DLCACHE/$filename" "$_md5" || {
        echo "$0: checksum mismatch!"
        exit 1
    }

    cp "$DLCACHE/$filename" "$target"
}

_locked "$DLCACHE/$(basename $1).locked" download "$@"
