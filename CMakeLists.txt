cmake_minimum_required(VERSION 3.16)
project(ft_malloc C)

set(CMAKE_SHARED_MODULE_SUFFIX .so)
set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(CMAKE_SHARED_LIBRARY_SUFFIX .so)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(libmalloc SHARED)

add_subdirectory(src)
target_include_directories(libmalloc PUBLIC
		${CMAKE_SOURCE_DIR}/inc
		)

set(HOSTTYPE $ENV{HOSTTYPE})

if ("${HOSTTYPE}" STREQUAL "")
	message("HOSTTYPE env var is not set, creating name...")
	execute_process(COMMAND uname -m
			OUTPUT_VARIABLE TYPE1
			OUTPUT_STRIP_TRAILING_WHITESPACE)
	execute_process(COMMAND uname -s
			OUTPUT_VARIABLE TYPE2
			OUTPUT_STRIP_TRAILING_WHITESPACE)
	set(HOSTTYPE "${TYPE1}_${TYPE2}")
else ()
	message("HOSTTYPE env var is \"${HOSTTYPE}\"")
endif ()

message("HOSTTYPE var is \"${HOSTTYPE}\"")

set(OUTNAME "libft_malloc_${HOSTTYPE}")
set(LINKNAME libft_malloc.so)
set_target_properties(libmalloc PROPERTIES
		OUTPUT_NAME "${OUTNAME}"
		ADDITIONAL_CLEAN_FILES "${LINKNAME}"
		)

target_compile_options(libmalloc PUBLIC
		-Wall
		-Werror
		-Wextra
		-g3
		-fsanitize=address
		)

target_link_options(libmalloc PUBLIC
		-fsanitize=address
		)

target_link_libraries(libmalloc ft_clib)
add_subdirectory(ft_clib EXCLUDE_FROM_ALL)

add_custom_command(POST_BUILD
		TARGET libmalloc
		COMMENT "creating symlink..."
		COMMAND ln -sf ${OUTNAME}.so ${LINKNAME})

add_executable(ft_malloc src/main.c)
target_link_libraries(ft_malloc libmalloc)