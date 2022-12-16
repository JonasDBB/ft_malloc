#!/usr/bin/env zsh

export DYLD_LIBRARY_PATH=.
export DYLD_INSERT_LIBRARIES="cmake-build-debug/libft_malloc.so"
export DYLD_FORCE_FLAT_NAMESPACE=1
$@
echo $?