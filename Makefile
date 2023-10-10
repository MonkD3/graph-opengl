SRCDIR=src
BUILDDIR=build

CCPP=g++
CC=gcc

SRCS_CPP=$(wildcard $(SRCDIR)/*.cpp)
OBJ_CPP=$(SRCS_CPP:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

SRCS_C=$(wildcard $(SRCDIR)/*.c)
OBJ_C=$(SRCS_C:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

INC=-I$(SRCDIR)/headers -I/usr/include/GLFW -I/usr/include/GL
LIB=-lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl

default: $(BUILDDIR)/graph

$(BUILDDIR)/graph: $(OBJ_C) $(OBJ_CPP) 
	$(CCPP) $(CFLAGS) $(INC) $(LIB) $^ -o $@

$(OBJ_CPP): $(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CCPP) $(CFLAGS) -c $< -o $@ $(INC) $(LIB)

$(OBJ_C): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LIB)

clean:
	rm $(BUILDDIR)/*

info:
	$(info $(SRCS_CPP))
	$(info $(OBJ_CPP))
	$(info $(SRCS_C))
	$(info $(OBJ_C))
	$(info $(BUILDDIR))
	$(info $(SRCDIR))

.PHONY: info
.PHONY: clean
