
all: a

a: main.c
	gcc -o a  main.c  -lavcodec -lavutil -lavformat -lswscale -g


clean:
	rm -f a


clean-frame:
	rm -f frames/*.jpeg