CC=gcc
CFLAGS=-Wall -std=gnu11 -O2
LDFLAGS=-lform -lncurses
INSTALLDIR?=/usr/local/bin

ODIR=bin
_BIN=iptables-cfg
BIN=$(patsubst %,$(ODIR)/%,$(_BIN))
_OBJ=$(_BIN).o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

IDIR=src
_DEPS=$(_BIN).h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

.PHONY: build clean install
build: $(BIN)
clean:
	rm -f $(ODIR)/*
install: build
	cp $(ODIR)/* $(INSTALLDIR)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@
	rm $(OBJ)
$(ODIR)/%.o: $(IDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
