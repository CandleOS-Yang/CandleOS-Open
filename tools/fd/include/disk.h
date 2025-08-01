#ifndef __FD_H__
#define __FD_H__

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SECTOR_SIZE 512                 // ÉÈÇø×Ö½ÚÊý

void disk_read(FILE *file, void *buf, u32 lba, u32 size);
void disk_write(FILE *file, void *buf, u32 lba, u32 size);
i64 get_file_size(FILE *file);
void write_file_to_disk(FILE *infile, FILE *outfile, u32 lba);

#endif