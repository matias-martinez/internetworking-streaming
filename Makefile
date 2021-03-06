EXECUTABLES = fuente server consumidor
DEP = src/utils.c src/tcpDataStreaming.c src/list.c src/out.c
SRCDIR = src
BINDIR = bin
INC = -I include
CXXFLAGS = -Wall -g
LIB = -lpthread

all: build $(EXECUTABLES)

build:
	@mkdir -p bin

$(EXECUTABLES):
	$(CC) $(INC) $(CXXFLAGS) $(SRCDIR)/$@.c  $(DEP) -o $(BINDIR)/$@ $(LIB)

clean:
	rm -rf $(BINDIR) 

.PHONY: clean
