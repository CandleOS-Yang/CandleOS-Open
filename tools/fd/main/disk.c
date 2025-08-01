#include "fat16.h"
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/* 读硬盘 LBA */
void disk_read(FILE *file, void *buf, u32 lba, u32 size) {
    fseek(file, lba * SECTOR_SIZE, SEEK_SET);
    fread(buf, 1, size, file);
}

/* 写硬盘 LBA */
void disk_write(FILE *file, void *buf, u32 lba, u32 size) {
    fseek(file, lba * SECTOR_SIZE, SEEK_SET);
    fwrite(buf, 1, size, file);
}

/* 获取文件大小 */
i64 get_file_size(FILE *file) {
    if (file == NULL) return -1;

    int fd = fileno(file);
    if (fd == -1) return -1;

    struct stat st;
    if (fstat(fd, &st) != 0) return -1;

    return st.st_size;
}

/* 程序功能1 写入文件到硬盘指定LBA扇区号 */
void write_file_to_disk(FILE *infile, FILE *outfile, u32 lba) {
    i64 size = get_file_size(infile);
    if (size == -1) {
        printf("[ ERROR ] Get file size failed!\n");
        return;
    }

    u32 sector_count = ceil(size, SECTOR_SIZE);
    printf("[ INFO ] File size: %lld\n", size);

    u8 *buf = (u8 *)malloc(sector_count * SECTOR_SIZE);
    memset((void *)buf, 0, sector_count * SECTOR_SIZE);

    disk_read(infile, buf, 0, size);
    disk_write(outfile, buf, lba, size);

    free(buf);
    printf("[ INFO ] Writed to disk successfully!\n");
}