EXECUTABLES = fuente server lista
SRCDIR = src
BINDIR = bin
INC = -I include

all: build $(EXECUTABLES)

build:
	@mkdir -p bin

fuente: build
	cc $(INC) $(SRCDIR)/fuente.c $(SRCDIR)/tcpDataStreaming.c -o $(BINDIR)/fuente

server: build
	cc $(INC) $(SRCDIR)/server.c $(SRCDIR)/tcpDataStreaming.c -o $(BINDIR)/server

list: build
	cc $(INC) $(SRCDIR)/list.c -o $(BINDIR)/list


clean:
	rm -rf $(BINDIR) 

.PHONY: clean
