CC=gcc
CFLAGS=-Wall -std=gnu11
LDFLAGS=-lncurses

ODIR=bin
_BIN=iptables-cfg
BIN=$(pathsubst %,$(ODIR)/%,$(_BIN))
_OBJ=$(_BIN).o
OBJ=$(pathsubst %,$(ODIR)/%,$(_OBJ))

IDIR=src
_DEPS=$(_BIN).h
DEPS=$(pathsubst %,$(IDIR)/%,$(_DEPS))

.PHONY: build clean
build: $(BIN)
clean:
	rm -f $(ODIR)/*

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@
$(ODIR)/%.o: $(IDIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
