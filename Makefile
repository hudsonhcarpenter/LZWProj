HEADERS = decode.h encode.h prune.h bit_pack.h
OBJECTS = program.o decode.o encode.o prune.o bit_pack.o

default: program

program.o: main.c $(HEADERS)
	gcc -c main.c -o program.o

decode.o: decode.c decode.h
	gcc -c decode.c -o decode.o

encode.o: encode.c encode.h
	gcc -c encode.c -o encode.o

prune.o: prune.c prune.h
	gcc -c prune.c -o prune.o

bit_pack.o: bit_pack.c bit_pack.h
	gcc -c bit_pack.c -o bit_pack.o

program: $(OBJECTS)
	touch encode
	touch decode
	rm -f decode
	rm -f encode
	
	gcc $(OBJECTS) -o program

	ln -s program decode
	ln -s program encode

clean:
	-rm -f $(OBJECTS)
	-rm -f program
	-rm -f decode
	-rm -f encode
	-rm -f DBG.encode
	-rm -f DBG.decode
	-rm -f prune
	-rm -f bit_pack
	-rm -f DBG.encode-p
	-rm -f DBG.decode-p