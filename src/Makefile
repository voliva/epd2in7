CC=g++
CFLAGS=-I.
DEPS = edif.h lut.h epd2in7.h
OBJ = epd2in7.o epdif.o lut.o main.o

LIBS=-lwiringPi

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ %< $(CFLAGS)

epd2in7: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

