#include "fat16.h"
#include "lib.h"
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

extern mbr_bios_block_t mbr_bios_block;             // MBR
extern partition_entry_t partition_table[4];        // 分区表
extern partition_info_t partition_info;             // 活动分区表

// 分区类型字符串
static char *partition_type[] = {
    "Empty",
    "FAT12(CHS)",
    "Unknown",
    "Unknown",
    "FAT16(CHS)",
    "Extended(CHS)",
    "FAT16(LBA)",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "FAT32(CHS)",
    "FAT32(LBA)"
};

/* 读取fat表 */
void read_fat(FILE *infile, u32 fat_num, void *fat_buffer) {
    disk_read(infile, fat_buffer, partition_info.fat_start + fat_num * partition_info.fat_sectors, partition_info.fat_sectors * SECTOR_SIZE);
}

/* 写入fat表 */
void write_fat(FILE *infile, u32 fat_num, void *fat_buffer) {
    disk_write(infile, fat_buffer, partition_info.fat_start + fat_num * partition_info.fat_sectors, partition_info.fat_sectors * SECTOR_SIZE);
}

/* 读取根目录 */
void read_rootdir(FILE *infile, void *rootdir_buffer) {
    disk_read(infile, rootdir_buffer, partition_info.rootdir_start, partition_info.rootdir_sectors * SECTOR_SIZE);
}

/* 写入根目录 */
void write_rootdir(FILE *infile, void *rootdir_buffer) {
    disk_write(infile, rootdir_buffer, partition_info.rootdir_start, partition_info.rootdir_sectors * SECTOR_SIZE);
}

/* 读取引导扇区 */
void read_mbr(FILE *infile, void *mbr_buffer) {
    disk_read(infile, mbr_buffer, 0, SECTOR_SIZE);
}

/* 读取DBR */
void read_dbr(FILE *infile, void *dbr_buffer) {
    disk_read(infile, dbr_buffer, partition_info.partition_start, SECTOR_SIZE);
}

/* 读取分区表 */
void read_partition(void *mbr_buffer) {
    for (int i = 0; i < MBR_PARTITION_COUNT; i++) {
        partition_table[i] = *((partition_entry_t *)((u8 *)mbr_buffer + PARTITION_OFFSET + i * PARTITION_SIZE));
    }
}

/* 输出分区信息 */
void print_partition_info(partition_info_t *partition_info) {
    printf("===================== MAIN PARTITION INFO =====================\n");
    printf("Partition Start:        %u\n", partition_info->partition_start);
    printf("Partition Sectors:      %u\n", partition_info->partition_sectors);
    printf("DBR Start:              %u\n", partition_info->dbr_start);
    printf("DBR Sectors:            %u\n", partition_info->dbr_sectors);
    printf("Reserved Sectors:       %u\n", partition_info->reserved_sectors);
    printf("FAT Start:              %u\n", partition_info->fat_start);
    printf("FAT Sectors:            %u\n", partition_info->fat_sectors);
    printf("FAT Count:              %u\n", partition_info->fat_count);
    printf("Root Directory Start:   %u\n", partition_info->rootdir_start);
    printf("Root Directory Sectors: %u\n", partition_info->rootdir_sectors);
    printf("Data Start:             %u\n", partition_info->data_start);
    printf("Data Sectors:           %u\n", partition_info->data_sectors);
    printf("Data Balance:           %u\n", partition_info->data_balance);
}

/* 获取分区信息 */
void get_partition_info(FILE *infile, u32 partition_num, partition_info_t *partition_info, mbr_bios_block_t *mbr_bios_block) {
    if (partition_num >= 4) {
        printf("[ ERROR ] Invalid partition number (0-3 allowed)\n");
        return;
    }
    
    read_mbr(infile, (void *)mbr_bios_block);

    read_partition((void *)mbr_bios_block);

    partition_entry_t *patition = &partition_table[partition_num];
    dbr_block_t *dbr = (dbr_block_t *)malloc(SECTOR_SIZE);
    if (dbr == NULL) {
        printf("[ ERROR ] Failed to allocate memory for dbr\n");
        return;
    }

    partition_info->partition_start = patition->start_lba;
    partition_info->partition_sectors = patition->sector_count;

    read_dbr(infile, (void *)dbr);

    partition_info->dbr_start = patition->start_lba;
    partition_info->dbr_sectors = 1;

    partition_info->reserved_sectors = dbr->BPB_RsvdSecCnt;

    partition_info->fat_start = partition_info->dbr_start + partition_info->reserved_sectors;
    partition_info->fat_sectors = dbr->BPB_FatSz16;
    partition_info->fat_count = dbr->BPB_NumFATs;

    partition_info->rootdir_start = partition_info->fat_start + partition_info->fat_sectors * partition_info->fat_count;
    partition_info->rootdir_sectors = partition_info->rootdir_sectors = (dbr->BPB_RootEntCnt * 32 + SECTOR_SIZE - 1) / SECTOR_SIZE;

    partition_info->data_start = partition_info->rootdir_start + partition_info->rootdir_sectors;
    partition_info->data_sectors = partition_info->partition_sectors - (partition_info->reserved_sectors + partition_info->fat_sectors * partition_info->fat_count + partition_info->rootdir_sectors);
    partition_info->data_balance = partition_info->data_start - 2;

    free(dbr);
}

/* 获取文件名称 */
void get_file_name(char *file_name, char *input_file_name) {
    char *p = strrchr(input_file_name, '/');

    if (p == NULL) {
        strcpy(file_name, input_file_name);
    } else {
        strcpy(file_name, p + 1);
    }
}

/* 转换文件名称格式 */
void convert_file_name(char *fat16_name, char *input_file_name) {
    memset(fat16_name, ' ', 11);

    char *ext = strrchr(input_file_name, '.');

    if (ext != NULL) {
        u32 name_len = ext - input_file_name;
        if (name_len > 8) {
            name_len = 8;
        }
        u32 ext_len = strlen(ext + 1);
        if (ext_len > 3) {
            ext_len = 3;
        }
        
        strncpy(fat16_name, input_file_name, name_len);
        strncpy(fat16_name + 8, ext + 1, ext_len);

        fat16_name[11] = '\0';
        strupr(fat16_name);
    } else {
        printf("[ ERROR ] Invalid file name: %s\n", input_file_name);
    }
}

/* 获取空闲簇号 */
u16 get_cluster(u16 *fat_buffer) {
    u32 clus_count = (partition_info.fat_sectors * SECTOR_SIZE) / 2;

    for (u32 i = 2; i < clus_count; i++) {
        if (fat_buffer[i] == 0x0000) {
            return i;
        }
    }
    return 0;
}

/* 链接簇号 */
u16 link_cluster(u16 *fat_buffer, u16 cluster) {
    u16 next_cluster = cluster + 1;
    u32 clus_count = (partition_info.fat_sectors * SECTOR_SIZE) / 2;

    for (u32 i = next_cluster; i < clus_count; i++) {
        if (fat_buffer[i] == 0x0000) {
            fat_buffer[cluster] = i;
            return i;
        }
    }
    return 0;
}

/* 获取目录项 */
dir_t *get_dir_entry(dir_t *rootdir_buffer) {
    u32 dir_count = (partition_info.rootdir_sectors * SECTOR_SIZE) / 32;
    for (u32 i = 0; i < dir_count; i++) {
        if (rootdir_buffer[i].DIR_Attr == 0) {
            return (void *)&rootdir_buffer[i];
        }
    }
    return NULL;
}

/* 功能2 写入文件到FAT16文件系统 */
void write_file_to_fat16(FILE *infile, FILE *outfile, u32 partition_num, char *input_file_name) {
    printf("[ INFO ] Writing file to FAT16 file system...\n");

    char file_fat_name[12];
    char file_name[12];

    u32 file_size = 0;
    u32 file_data_count = 0;
    u16 file_start_cluster = 0;
    u16 cluster = 0;
    u32 file_data_start = 0;

    dbr_block_t *dbr = NULL;
    u16 *fat_buffer = NULL;
    u16 *fat2_buffer = NULL;
    dir_t *rootdir_buffer = NULL;
    u8 *data_buffer = NULL;
    dir_t *dir_entry = NULL;
    u8 *data_buffer_ptr = NULL;

    get_partition_info(outfile, partition_num, &partition_info, &mbr_bios_block);
    
    get_file_name(file_name, input_file_name);
    convert_file_name(file_fat_name, file_name);
    
    file_size = get_file_size(infile);
    file_data_count = ceil(file_size, SECTOR_SIZE);

    fat_buffer = (u16 *)malloc(partition_info.fat_sectors * SECTOR_SIZE);
    fat2_buffer = (u16 *)malloc(partition_info.fat_sectors * SECTOR_SIZE);
    rootdir_buffer = (dir_t *)malloc(partition_info.rootdir_sectors * SECTOR_SIZE);
    dbr = (dbr_block_t *)malloc(SECTOR_SIZE);
    data_buffer = (u8 *)malloc(file_data_count * SECTOR_SIZE);

    if (fat_buffer == NULL) {
        printf("[ ERROR ] Failed to allocate memory for fat_buffer\n");
        return;
    }
    if (fat2_buffer == NULL) {
        printf("[ ERROR ] Failed to allocate memory for fat2_buffer\n");
        return;
    }
    if (rootdir_buffer == NULL) {
        printf("[ ERROR ] Failed to allocate memory for rootdir_buffer\n");
        return;
    }
    if (dbr == NULL) {
        printf("[ ERROR ] Failed to allocate memory for dbr\n");
        return;
    }
    if (data_buffer == NULL) {
        printf("[ ERROR ] Failed to allocate memory for data_buffer\n");
        return;
    }
    memset(fat_buffer, 0, partition_info.fat_sectors * SECTOR_SIZE);
    memset(fat2_buffer, 0, partition_info.fat_sectors * SECTOR_SIZE);
    memset(rootdir_buffer, 0, partition_info.rootdir_sectors * SECTOR_SIZE);
    memset(data_buffer, 0, file_data_count * SECTOR_SIZE);

    read_fat(outfile, 0, (void *)fat_buffer);
    read_rootdir(outfile, (void *)rootdir_buffer);
    read_dbr(infile, (void *)dbr);

    fat_buffer[0] = 0xfff8;
    fat_buffer[1] = 0xffff;

    file_start_cluster = get_cluster(fat_buffer);
    dir_entry = get_dir_entry((dir_t *)rootdir_buffer);

    if (file_start_cluster == 0) {
        printf("[ ERROR ] No free cluster\n");
        return;
    }
    if (dir_entry == NULL) {
        printf("[ ERROR ] No free directory entry\n");
        return;
    }
    file_data_start = partition_info.data_balance + file_start_cluster;

    strncpy((char *)dir_entry->DIR_Name, file_fat_name, 8);
    strncpy((char *)dir_entry->DIR_Ext, file_fat_name + 8, 3);

    dir_entry->DIR_FstClus = file_start_cluster;
    dir_entry->DIR_FileSize = file_size;

    // 目录项时间属性补充
    
    if (strcmp(file_fat_name + 8, "   ")) {     // 没有扩展名，文件
        dir_entry->DIR_Attr = 0x20;
    } else {                                    // 有扩展名，目录
        dir_entry->DIR_Attr = 0x10;
    } // 其他判断

    cluster = file_start_cluster;
    data_buffer_ptr = data_buffer;
    for (u32 i = 0; i < file_data_count; i++) {
        disk_read(infile, data_buffer_ptr, i, file_size);
        disk_write(outfile, data_buffer_ptr, file_data_start, SECTOR_SIZE);

        cluster = link_cluster(fat_buffer, cluster);
        if (cluster == 0) {
            printf("[ ERROR ] Failed to link cluster\n");
            return;
        }

        file_data_start = partition_info.data_balance + cluster;
        data_buffer_ptr += SECTOR_SIZE;
    }
    fat_buffer[cluster] = 0xffff;

    memcpy(fat2_buffer, fat_buffer, partition_info.fat_sectors * SECTOR_SIZE);

    write_fat(outfile, 0, (void *)fat_buffer);
    write_fat(outfile, 1, (void *)fat2_buffer);
    write_rootdir(outfile, (void *)rootdir_buffer);

    printf("[ INFO ] File size: %d\n", file_size);
    printf("[ INFO ] FAT16 file name: %s\n", file_fat_name);
    printf("[ INFO ] File start cluster: %d\n", file_start_cluster);
    printf("[ INFO ] Data base: 0x%x\n", (file_data_start - file_data_count) * SECTOR_SIZE);

    free(fat_buffer);
    free(rootdir_buffer);
    free(dbr);
    free(data_buffer);

    printf("[ INFO ] Writed file to FAT16 file system successfully!\n");
}

/* 功能3 输出分区表信息 */
void print_partition_table(FILE *infile, mbr_bios_block_t *mbr_bios_block) {
    read_mbr(infile, (void *)mbr_bios_block);

    read_partition((void *)mbr_bios_block);

    printf("===================== PARTITION ENTRY =====================\n");
    for (int i = 0; i < MBR_PARTITION_COUNT; i++) {
        printf("[ INFO ] Partition %d\n", i);
        printf("[ INFO ] Boot Indicator: 0x%02X\n", partition_table[i].boot_indicator);
        printf("[ INFO ] File System: 0x%02X(%s)\n", partition_table[i].partition_type, partition_type[partition_table[i].partition_type]);
        printf("[ INFO ] Base: 0x%x\n", partition_table[i].start_lba * SECTOR_SIZE);
        printf("[ INFO ] Size: %d(=%d MB)\n\n", partition_table[i].sector_count * SECTOR_SIZE, partition_table[i].sector_count * SECTOR_SIZE / (1024 * 1024));
    }
}