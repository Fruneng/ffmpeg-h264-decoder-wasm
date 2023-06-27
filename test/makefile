
all: testbin

testbin: ../src/decoder.c
	gcc -o testbin  ../src/decoder.c  -lavcodec -lavutil -lavformat -lswscale -g


clean:
	rm -f testbin
