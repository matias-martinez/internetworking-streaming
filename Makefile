EXECUTABLES = fuente server fuente2 consumidor
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

fuente2: build
	cc $(INC) $(CXXFLAGS) $(SRCDIR)/fuente2.c $(SRCDIR)/tcpDataStreaming.c -o $(BINDIR)/fuente2

consumidor: build

	cc $(INC) $(CXXFLAGS) $(SRCDIR)/consumidor.c $(SRCDIR)/tcpDataStreaming.c -o $(BINDIR)/consumidor

clean:
	rm -rf $(BINDIR) 

.PHONY: clean
