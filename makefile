
all: a

a: main.c
	gcc -o a  main.c  -lavcodec -lavutil -lavformat -lswscale


clean:
	rm -f a
	rm -f *.jpeg