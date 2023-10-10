SRCDIR=src
BUILDDIR=build

SRCS=$(wildcard $(SRCDIR)/*.c)
OBJ=$(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

INC=-I$(SRCDIR)/headers -I/usr/include/GLFW -I/usr/include/GL
LIB=-lGL -lglfw -lX11 -lpthread -lXrandr -lXi -ldl

default: $(BUILDDIR)/graph

$(BUILDDIR)/graph: $(OBJ)
	$(CC) $(CFLAGS) $(INC) $(LIB) $^ -o $@

$(OBJ): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC) $(LIB)

clean:
	rm $(BUILDDIR)/*

info:
	$(info $(SRCS))
	$(info $(OBJ))
	$(info $(BUILDDIR))
	$(info $(SRCDIR))

.PHONY: info
.PHONY: clean
