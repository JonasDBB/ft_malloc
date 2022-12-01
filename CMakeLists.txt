cmake_minimum_required(VERSION 3.16)
project(ft_malloc C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(ft_malloc)

add_subdirectory(src)

include_directories(
		${CMAKE_SOURCE_DIR}/inc
)

target_compile_options(ft_malloc PUBLIC
		-Wall
		-Werror
		-Wextra
		-g3
		-fsanitize=address
		)

target_link_options(ft_malloc PUBLIC
		-fsanitize=address
		)

add_subdirectory(ft_clib EXCLUDE_FROM_ALL)
target_link_libraries(ft_malloc ft_list ft_clib)