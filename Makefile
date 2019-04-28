.POSIX:

include config.mk

SRC = fgfind.c unicode.c
OBJ = $(SRC:.c=.o)

all: options fgfind
	
options:
	@echo fgfind build options:
	@echo "CFLAGS  = $(FGFCFLAGS)"
	@echo "LDFLAGS = $(FGFLDFLAGS)"
	@echo "CC      = $(CC)"

.c.o:
	$(CC) $(FGFCFLAGS) -c $<
	
$(OBJ): config.mk

fgfind: $(OBJ)
	$(CC) -o $@ $(OBJ) $(FGFLDFLAGS)
	
clean:
	rm -f fgfind $(OBJ)

install: fgfind
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f fgfind $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/fgfind

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/fgfind
