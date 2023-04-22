CFLAGS=-lsqlite3 -Ofast
CSRC=subete.c
CLIBS=ctodo.c
LIBS=todo.o
INCLUDES = -I .
INSTALL   ?= install
MKDIR     ?= $(INSTALL) -d
BINDIR    ?= $(PREFIX)/bin
DESTDIR   ?=

all: subete

rebuild: clean all

subete: $(CSRC) $(LIBS)
    $(CC) ${INCLUDES} -o subete $^ ${CFLAGS}

$(LIBS): $(CLIBS)
    $(CC) ${INCLUDES} -c -o $@ $^ $(CFLAGS)

install: subete
    $(MKDIR) $(DESTDIR)$(BINDIR)
    $(INSTALL) subete $(DESTDIR)$(BINDIR)

uninstall:
    rm -f $(DESTDIR)$(BINDIR)/subete

clean:
	@echo "Cleaning binaries..."
	rm -f ${LIBS}
	rm -f subete
	@echo "Cleaning temp files..."
	find . -name '*~' -delete;
	find . -name '#*#' -delete;
	@echo "Cleaning complete."
