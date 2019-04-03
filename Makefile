CFLAGS=$(shell pkg-config --cflags --libs sdl) -Wall -Wextra
OBJ:=binclock

all: $(OBJ)

clean:
	rm -f $(OBJ)
