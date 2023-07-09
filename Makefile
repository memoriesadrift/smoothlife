CC = gcc
CFLAGS = -Wall -Wextra -O3
CLINKS = -lm
OBJ = life.x

main:
	$(CC) $(CFLAGS) main.c -o $(OBJ) $(CLINKS)

exec:
	./$(OBJ)

clean: 
	rm $(OBJ)
