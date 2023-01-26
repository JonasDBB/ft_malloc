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
	realloc.c \
	show_alloc_mem.c

_OBJS = $(addprefix $(OBJDIR), $(_SRCS))

INCS = $(addprefix $(INCDIR), $(_INCS))
SRCS = $(addprefix $(SRCDIR), $(_SRCS))
OBJS = $(_OBJS:.c=.o)

all: git-submodules $(NAME)

git-submodules:
	@git submodule update --init

# this was made with cmake, so manually making it here
CLIB_NAME := libft_clib.a
CLIB_DIR := ft_clib/
CLIB_SRCDIR := ft_clib/
CLIB_C := ft_clib.c
CLIB_O := $(CLIB_C:.c=.o)
$(OBJDIR)$(CLIB_NAME):
	@echo "creating clib"
	@mkdir -p $(OBJDIR)
	@cd $(OBJDIR) && \
	$(CC) $(CFLAGS) -c ../$(CLIB_DIR)$(CLIB_SRCDIR)$(CLIB_C) && \
	ar rcs $(CLIB_NAME) $(CLIB_O)

$(OBJDIR)%.o: $(SRCDIR)%.c $(OBJDIR)$(CLIB_NAME) $(INCDIR)*.h
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

test0: all
	gcc -o test0 tests/test0.c
	./run.sh /usr/bin/time -l ./test0
	rm test0

test1: all
	gcc -o test1 tests/test1.c
	./run.sh /usr/bin/time -l ./test1
	rm test1

test2: all
	gcc -o test2 tests/test2.c
	./run.sh /usr/bin/time -l ./test2
	rm test2

test3: all
	gcc -o test3 tests/test3.c
	./run.sh /usr/bin/time -l ./test3
	rm test3

test4: all
	gcc -o test4 tests/test4.c
	./run.sh /usr/bin/time -l ./test4
	rm test4

test5: all
	gcc -o test5 tests/test5.c
	./run.sh ./test5
	rm test5

test6: all
	gcc -o test6 tests/test6.c $(LINKNAME) -I $(INCDIR) -I $(CLIB_DIR)$(CLIB_SRCDIR)
	./run.sh ./test6
	rm test6
