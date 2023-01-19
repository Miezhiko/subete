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

rebuild: clean | all

subete: $(CSRC) | ctodo
	$(CC) ${INCLUDES} -o subete $^ ${CFLAGS} ${LIBS}

ctodo: $(CLIBS)
	$(CC) ${INCLUDES} -c -o ${LIBS} $^ $(CFLAGS)

.PHONY: clean all rebuild inheritsqlite

clean:
	@echo "Cleaning binaries..."
	rm -f ${LIBS}
	rm -f subete
	@echo "Cleaning temp files..."
	find . -name '*~' -delete;
	find . -name '#*#' -delete;
	@echo "Cleaning complete."

install:
	$(MKDIR) $(DESTDIR)$(BINDIR)
	$(INSTALL) subete$(EXE) $(DESTDIR)$(BINDIR)/
