CC     = cc
CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined 
SRC    = $(wildcard src/*.c)

myshell: $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -f myshell
