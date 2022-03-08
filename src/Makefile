CFLAGS := -std=c11 -m32 -nostdlib -ffreestanding -Wall -Wextra -pedantic -O2 -ffast-math

.PHONY: all clean

all: raytracer

raytracer: entry.o main.o common.o tiny_stdlib.o alloc.o sphere.o material.o camera.o hittable.o renderer.o aabb.o bvh.o
	$(CC) $(CFLAGS) -Wl,-Tlinker.ld -o '$@' $^

entry.o: entry.S
	$(CC) $(CFLAGS) -c -o '$@' '$<'

clean:
	rm -f raytracer *.o
