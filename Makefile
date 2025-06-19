CC= gcc#compiler

TARGET = ortografia #target file name

all:
	$(CC) -Wall -O3 -g -o $(TARGET) ortografia.c aux.c

clean:
	rm $(TARGET)
