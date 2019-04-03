CFLAGS=$(shell pkg-config --cflags --libs sdl)
OBJ:=binclock

all: $(OBJ)

clean:
	rm -f $(OBJ)
