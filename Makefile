.PHONY: clean run
.SILENT:

cube.o: cube.c
	gcc -o $@ $< -lm -lncurses

run: cube.o
	./$<

clean:
	rm -rf cube.o
