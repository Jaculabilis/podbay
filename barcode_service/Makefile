CC=gcc
CFLAGS=-O2 -Wall -Wextra
LDFLAGS=-lzmq -lczmq
EXE_NAME=barcode_service

all: $(EXE_NAME)

$(EXE_NAME): barcode_service.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f $(EXE_NAME)
