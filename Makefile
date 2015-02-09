EXECUTABLES = fuente server
SRCDIR = src
BINDIR = bin
INC = -I include

all: build $(EXECUTABLES)

build:
	@mkdir -p bin

fuente: 
	cc $(INC) $(SRCDIR)/fuente.c $(SRCDIR)/tcpDataStreaming.c -o $(BINDIR)/fuente

server:
	cc $(INC) $(SRCDIR)/server.c $(SRCDIR)/tcpDataStreaming.c -o $(BINDIR)/server

clean:
	rm -rf $(BINDIR) 

.PHONY: clean
