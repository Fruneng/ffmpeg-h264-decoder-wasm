all: build-prd

CFLAGS := -O3 -msimd128

build-prd:
	EXTRA_CFLAGS="$(CFLAGS)" docker build . --build-arg EXTRA_CFLAGS  -o .  

build-dev:
	docker build .  -o .  