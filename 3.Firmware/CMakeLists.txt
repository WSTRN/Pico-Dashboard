cmake_minimum_required(VERSION 3.13)
include(FetchContent)

# initialize pico-sdk from submodule
# note: this must happen before project()
include(pico-sdk/pico_sdk_init.cmake)

project(Pico-Dashboard)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_definitions(-DLFS_THREADSAFE -DCFG_MSC_READONLY)
file(GLOB_RECURSE SRC_LIST 
	"UserApp/*.cpp"
	"UserApp/*.c"
	"LCD/*.c"
	"USB/*.c"
	"Src/*.c"
	"LFS/*.c"
	"Protocol/*.c"
)
add_executable(Pico-Dashboard ${SRC_LIST})

# Specify path to own LVGL config header
set(LV_CONF_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/Inc/lv_conf.h
    CACHE STRING "" FORCE)
# Fetch LVGL from GitHub
FetchContent_Declare(lvgl 
	GIT_REPOSITORY https://github.com/lvgl/lvgl.git
	GIT_TAG v8.3.4
)
FetchContent_MakeAvailable(lvgl)

include_directories(
	Inc
	LCD
	USB
	LFS
	Protocol
)

target_link_libraries(Pico-Dashboard
	PRIVATE lvgl::lvgl
	hardware_spi
	hardware_dma
	hardware_pwm
	hardware_flash
	pico_stdlib
	pico_malloc
	pico_util
	pico_multicore
	tinyusb_device
	tinyusb_board
)
target_link_options(Pico-Dashboard PRIVATE
	-Wl,--print-memory-usage
	)


# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(Pico-Dashboard)

