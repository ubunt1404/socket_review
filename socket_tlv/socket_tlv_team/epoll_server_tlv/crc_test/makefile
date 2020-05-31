VAR= crc-itu-t.o crc_test.o
test:$(VAR)
	gcc -o test $(VAR)
%.o:%.c
	gcc -c $< -o $@
.PHONY:clean
clean:
	rm -rf *.o test
