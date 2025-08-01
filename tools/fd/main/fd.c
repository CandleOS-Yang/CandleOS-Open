#include "fat16.h"
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/*
基本格式：fd [input] -o [output] -c [command] -x [other]

1. 写入文件到硬盘指定LBA扇区号
fd [input] -o [output] -c -w [lba]

2. 写入文件系统
fd [input] -o [output] -c -wfs -p [partition] -d [directory]

3. 显示分区信息
fd [input] -c -p [partition]
*/

mbr_bios_block_t mbr_bios_block;            // MBR
partition_entry_t partition_table[4];       // 分区表
partition_info_t partition_info;            // 活动分区信息

/* 主函数 */
int main(int argc, char *argv[]) {
    FILE *infile;
    FILE *outfile;

    if (argc == 7) {
        // fd [input] -o [output] -c -w [lba]
        if (strcmp(argv[2], "-o") == 0 && strcmp(argv[4], "-c") == 0 && strcmp(argv[5], "-w") == 0) {
            infile = fopen(argv[1], "rb");
            outfile = fopen(argv[3], "rb+");
            if (infile == NULL) {
                printf("[ ERROR ] Please check your input file!\n");
                return -1;
            } 
            if (outfile == NULL) {
                printf("[ ERROR ] Please check your output file!\n");
                return -1;
            }

            write_file_to_disk(infile, outfile, atoi(argv[6]));
            
            if (infile != NULL) {
                fclose(infile);
            }
            if (outfile != NULL) {
                fclose(outfile);
            }
        } else {
            printf("[ ERROR ] Please check your input parameters!\n");
        }
    } else if (argc == 10) {
        // fd [input] -o [output] -c -wfs -p [partition] -d [directory]
        if (strcmp(argv[2], "-o") == 0 && strcmp(argv[4], "-c") == 0 && strcmp(argv[5], "-wfs") == 0 && strcmp(argv[6], "-p") == 0 && strcmp(argv[8], "-d") == 0) {
            infile = fopen(argv[1], "rb");
            outfile = fopen(argv[3], "rb+");
            if (infile == NULL) {
                printf("[ ERROR ] Please check your input file!\n");
                return -1;
            } 
            if (outfile == NULL) {
                printf("[ ERROR ] Please check your output file!\n");
                return -1;
            }

            u32 partition_num = atoi(argv[7]);
            if (partition_num < 0) {
                printf("[ ERROR ] Please check your partition number!\n");
                return -1;
            }

            write_file_to_fat16(infile, outfile, partition_num, argv[1]);

            if (infile != NULL) {
                fclose(infile);
            }
            if (outfile != NULL) {
                fclose(outfile);
            }
        } else {
            printf("[ ERROR ] Please check your input parameters!\n");
        }
    } else if (argc == 5) {
        // fd [input] -c -p [partition]
        if (strcmp(argv[2], "-c") == 0 && strcmp(argv[3], "-p") == 0) {
            infile = fopen(argv[1], "rb");
            if (infile == NULL) {
                printf("[ ERROR ] Please check your input file!\n");
                return -1;
            }

            print_partition_table(infile, (void *)&mbr_bios_block);
            get_partition_info(infile, atoi(argv[4]), (void *)&partition_info, (void *)&mbr_bios_block);
            print_partition_info((void *)&partition_info);
            
            if (infile != NULL) {
                fclose(infile);
            }
        } else {
            printf("[ ERROR ] Please check your input parameters!\n");
        }
    } else {
        printf("[ ERROR ] Please check your input parameters!\n");
    }
}