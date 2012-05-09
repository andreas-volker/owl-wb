PROGRAM = owl-browser
VERSION = 0.1.0

PREFIX = $(DESTDIR)/usr
BINDIR = $(PREFIX)/bin

CC = cc
LD = $(CC)

INCS = -I. -I/usr/include
INCS+= `pkg-config --cflags gtk+-2.0 webkit-1.0`
LIBS = -L/usr/lib -lc
LIBS+= `pkg-config --libs  gtk+-2.0 webkit-1.0`

CPPFLAGS+= -DVERSION=\"$(VERSION)\" -DPROGRAM=\"$(PROGRAM)\"
CFLAGS   = -std=c99 -W -Wall -Wextra -Wshadow -pedantic $(CPPFLAGS) $(INCS)
LDFLAGS  = $(LIBS)

DEBUG = FALSE
ifeq ($(DEBUG), TRUE)
	CFLAGS += -g
	LDFLAGS+= -g
else
	CFLAGS += -Os
	LDFLAGS+= -s
endif

SRC = main.c
OBJ = $(SRC:.c=.o)

RM = rm -vf

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $<

$(OBJ): $(SRC) *.c
	$(CC) $(CFLAGS) -c $<

clean:
	@$(RM) $(PROGRAM) $(OBJ)

install: all
	@mkdir -p $(BINDIR)
	@cp -fv $(PROGRAM) $(BINDIR)
	@chmod 755 $(BINDIR)/$(PROGRAM)

uninstall:
	@$(RM) $(BINDIR)/$(PROGRAM)

.PHONY: all clean install uninstall
