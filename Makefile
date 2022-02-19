SRC_DIR := src

.PHONY: all clean run $(SRC_DIR)/raytracer

all: disk_image

disk_image: $(SRC_DIR)/raytracer
	cp '$<' iso/boot
	grub-mkrescue -o '$@' iso

$(SRC_DIR)/raytracer:
	@cd $(SRC_DIR) && $(MAKE) raytracer

clean:
	@cd $(SRC_DIR) && $(MAKE) $@
	rm -f iso/boot/raytracer disk_image

run: disk_image
	./run.sh
