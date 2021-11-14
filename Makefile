CC=gcc
CCFLAGS=-Wall -g
LDFLAGS=
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TARGET=locker
COMPNAME=locker

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) -o $@ $^ $(LDFLAGS) 

%.o: %.c %.h
		$(CC) $(CCFLAGS) -c $<

%.o: %.c
		$(CC) $(CCFLAGS) -c $<

clean:
		rm -f *.o $(TARGET)

wc: 
		wc -l *.c *.h

zip:
		zip $(COMPNAME).tp5.zip *.c *.h Makefile README.md

tar:
		tar -cvzf $(COMPNAME).tp5.tar *.c *.h Makefile README.md

delzip:
		rm $(COMPNAME).tp5.zip

deltar: 
		rm $(COMPNAME).tp5.tar