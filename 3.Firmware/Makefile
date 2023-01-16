.PHONY: all build cmake clean format

BUILD_DIR := build

all: build

cmake:
	cmake \
		-B${BUILD_DIR} \
		-DCMAKE_BUILD_TYPE=Release \
		-DPICO_BOARD_HEADER_DIRS=$(CURDIR)/Inc \
		-DPICO_BOARD=waveshare_rp2040_lcd_1.28 \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON 

build: cmake
	$(MAKE) -C ${BUILD_DIR} -j

clean:
	rm -rf $(BUILD_DIR)
