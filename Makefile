EXECUTABLES = fuente server
SRCDIR = src
BINDIR = bin
INC = -I include
CXXFLAGS = -g

all: build $(EXECUTABLES)

build:
	@mkdir -p bin

fuente: build
	cc $(INC) $(CXXFLAGS) $(SRCDIR)/fuente.c $(SRCDIR)/tcpDataStreaming.c -o $(BINDIR)/fuente

server: build
	cc $(INC) $(CXXFLAGS) $(SRCDIR)/server.c $(SRCDIR)/tcpDataStreaming.c $(SRCDIR)/list.c -o $(BINDIR)/server -lpthread



clean:
	rm -rf $(BINDIR) 

.PHONY: clean
