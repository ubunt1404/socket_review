get_temp: temperature.o cline_01.o
	gcc -o get_temp temperature.o cline_01.o
temperature.o:temperature.c
	gcc -c temperature.c -o temperature.o
cline_01.o:cline_01.c
	gcc -c cline_01.c -o cline_01.o
.PHONY:clean
clean:
	-rm -rf *.o get_temp test a.out	
