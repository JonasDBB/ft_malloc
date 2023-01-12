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

# this was made with cmake, so manually making it here
CLIB_NAME := libft_clib.a
CLIB_DIR := ft_clib/
CLIB_SRCDIR := ft_clib/
CLIB_C := ft_clib.c
CLIB_O := $(CLIB_C:.c=.o)
$(OBJDIR)$(CLIB_NAME):
	@mkdir -p $(OBJDIR)
	cd $(OBJDIR) && \
	$(CC) $(CFLAGS) -c ../$(CLIB_DIR)$(CLIB_SRCDIR)$(CLIB_C) && \
	ar rcs $(CLIB_NAME) $(CLIB_O)

$(OBJDIR)%.o: $(SRCDIR)%.c $(OBJDIR)$(CLIB_NAME)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -fPIC $< -o $@ -I$(INCDIR) -I$(CLIB_DIR)$(CLIB_SRCDIR)

$(NAME): $(OBJS) $(INCS)
	$(CC) $(CFLAGS) $(OBJS) $(OBJDIR)$(CLIB_NAME) -shared -o $(NAME)
	ln -sf $(NAME) $(LINKNAME)

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)
	@rm -f $(LINKNAME)

re: fclean all
