CC=g++
CFLAGS=-pthread

all:
	$(CC) $(CFLAGS) code.cpp -o wifitool
clean:
	rm -rf wifitool
