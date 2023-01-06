ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

SHARED_LIB_EXT = so

NAME := libft_malloc_$(HOSTTYPE).$(SHARED_LIB_EXT)
LINKNAME := libft_malloc.$(SHARED_LIB_EXT)
CC := gcc
CFLAGS = \
	-Wall \
	-Wextra \
	-Werror

ifdef DEBUG
CFLAGS += \
	-g3 \
	-fsanitize=address
endif

SRCDIR := src/
INCDIR := inc/
OBJDIR := obj/

_INCS = \
	ft_malloc.h \
	ft_malloc_private.h

_SRCS = \
	malloc.c \
	free.c \
	realloc.c

_OBJS = $(addprefix $(OBJDIR), $(_SRCS))

INCS = $(addprefix $(INCDIR), $(_INCS))
SRCS = $(addprefix $(SRCDIR), $(_SRCS))
OBJS = $(_OBJS:.c=.o)

all: $(NAME)

check-and-reinit-submodules:
	@git config --global --add safe.directory '*'
	@if git submodule status | egrep -q '^[-]|^[+]' ; then \
			git submodule update --init; \
	fi
CLIB_NAME := libft_clib.a
CLIB_DIR := ft_clib/
CLIB_SRCDIR := ft_clib/
CLIB_BUILDDIR := build/
CLIB := $(CLIB_DIR)$(CLIB_BUILDDIR)$(CLIB_NAME)
$(CLIB): check-and-reinit-submodules
	@cd $(CLIB_DIR) && \
	mkdir -p $(CLIB_BUILDDIR) && \
	cmake -B $(CLIB_BUILDDIR) -S . && \
	cmake --build $(CLIB_BUILDDIR) --target ft_clib

$(OBJDIR)%.o: $(SRCDIR)%.c $(CLIB)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -fPIC $< -o $@ -I$(INCDIR) -I$(CLIB_DIR)$(CLIB_SRCDIR)

$(NAME): $(OBJS) $(INCS)
	$(CC) $(CFLAGS) $(OBJS) $(CLIB) -shared -o $(NAME)
	ln -sf $(NAME) $(LINKNAME)

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)
	@rm -f $(LINKNAME)
	@rm -rf $(CLIB_DIR)$(CLIB_BUILDDIR)

re: fclean all
