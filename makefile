CC=g++
CFLAGS=

All: makler

.SUFFIXES: .cpp
.cpp.o:
	$(CC) -xc++ $(CFLAGS) -c $<

makler: common.o cportfel.o cspolka.o cwolumen.o get_nasdaq.o get_wig20.o main.o
	$(CC) common.o cportfel.o cspolka.o cwolumen.o get_nasdaq.o get_wig20.o main.o $(CFLAGS) -o makler

common.o: common.cpp main.h

cportfel.o:	cportfel.cpp cspolka.h cportfel.h cwolumen.h main.h

cspolka.o: cspolka.cpp cwolumen.h main.h cspolka.h

cwolumen.o: cwolumen.cpp cwolumen.h main.h

get_nasdaq.o: get_nasdaq.cpp main.h get_nasdaq.h

get_wig20.o: get_wig20.cpp main.h get_wig20.h

main.o: main.cpp main.h cportfel.h cspolka.h cwolumen.h get_wig20.h get_nasdaq.h

clean:
	rm -f common.o 
	rm -f cportfel.o 
	rm -f cspolka.o 
	rm -f cwolumen.o 
	rm -f get_nasdaq.o 
	rm -f get_wig20.o 
	rm -f main.o
	rm -f *~
	rm -f #*
	rm -f *#
