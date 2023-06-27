
all: a

a: decoder.c
	gcc -o a  decoder.c  -lavcodec -lavutil -lavformat -lswscale -g


clean:
	rm -f a


clean-frame:
	rm -f frames/*.jpeg