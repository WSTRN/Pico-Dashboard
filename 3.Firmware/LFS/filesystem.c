#include "filesystem.h"
#include "hardware/flash.h"
#include "hardware/regs/addressmap.h"
#include "pico/stdio.h"
#include "usbcdc.h"
#include "stdio.h"

// 128K of space for file system at top of pico flash
#define FS_SIZE (1024 * 1024)

// hal function prototypes
static int pico_read(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer,
                     lfs_size_t size);
static int pico_prog(const struct lfs_config* c, lfs_block_t block, lfs_off_t off,
                     const void* buffer, lfs_size_t size);
static int pico_erase(const struct lfs_config* c, lfs_block_t block);
static int pico_sync(const struct lfs_config* c);
static int pico_lock(const struct lfs_config *c);
static int pico_unlock(const struct lfs_config *c);


// configuration of the filesystem is provided by this struct
// for Pico: prog size = 256, block size = 4096, so cache is 8K
// minimum cache = block size, must be multiple
const struct lfs_config cfg = {
    // block device operations
    .read = pico_read,
    .prog = pico_prog,
    .erase = pico_erase,
    .sync = pico_sync,
#if LFS_THREADSAFE
    .lock = pico_lock,
    .unlock = pico_unlock,
#endif
    // block device configuration
    .read_size = 1,
    .prog_size = FLASH_PAGE_SIZE,
    .block_size = FLASH_SECTOR_SIZE,
    .block_count = FS_SIZE / FLASH_SECTOR_SIZE,
    .cache_size = FLASH_SECTOR_SIZE,
    .lookahead_size = 16,
    .block_cycles = 500,
};

// file system offset in flash
static const uint32_t fs_base = PICO_FLASH_SIZE_BYTES - FS_SIZE;

static int pico_read(const struct lfs_config* c, lfs_block_t block, lfs_off_t off, void* buffer,
                     lfs_size_t size) {
    (void)c;
    // read flash via XIP mapped space
    uint8_t* p = (uint8_t*)(XIP_NOCACHE_NOALLOC_BASE + fs_base + (block * FLASH_SECTOR_SIZE) + off);
    memcpy(buffer, p, size);
    return 0;
}

static int pico_prog(const struct lfs_config* c, lfs_block_t block, lfs_off_t off,
                     const void* buffer, lfs_size_t size) {
    (void)c;
    uint32_t p = (block * FLASH_SECTOR_SIZE) + off;
    // program with SDK
    flash_range_program(fs_base + p, buffer, size);
    return 0;
}

static int pico_erase(const struct lfs_config* c, lfs_block_t block) {
    (void)c;
    uint32_t off = block * FLASH_SECTOR_SIZE;
    // erase with SDK
    flash_range_erase(fs_base + off, FLASH_SECTOR_SIZE);
    return 0;
}

static int pico_sync(const struct lfs_config* c) {
    (void)c;
    // nothing to do!
    return 0;
}

#if LIB_PICO_MULTICORE

static recursive_mutex_t fs_mtx;

static int pico_lock(const struct lfs_config *c) {
    recursive_mutex_enter_blocking(&fs_mtx);
    return LFS_ERR_OK;
}

static int pico_unlock(const struct lfs_config *c) {
    recursive_mutex_exit(&fs_mtx);
    return LFS_ERR_OK;
}
#endif


// increment the boot count with each invocation
lfs_size_t boot_count;

void LittleFS_init()
{
    // variables used by the filesystem
    lfs_t lfs;
    lfs_file_t file;

    // initialize the pico SDK
    /*stdio_init_all();*/
    /*printf("\033[H\033[J"); // try to clear the screen*/

    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        /*printf("1st time formatting\n");*/
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
        // create the boot count file
        boot_count = 0;
        lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
        lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));
        lfs_file_close(&lfs, &file);
    }
    // read current count
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    /*printf("boot_count: %d\n", (int)boot_count);*/
}
