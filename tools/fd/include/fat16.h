#ifndef __FAT16_H__
#define __FAT16_H__

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MBR_PARTITION_COUNT 4           // MBR分区表项个数
#define PARTITION_SIZE  16              // MBR分区表项大小
#define PARTITION_OFFSET 0x1BE          // MBR分区表偏移

#define FILE_ATTR_RW         0x0        // 读写
#define FILE_ATTR_READ_ONLY  0x1        // 只读
#define FILE_ATTR_HIDDEN     0x2        // 隐藏
#define FILE_ATTR_SYSTEM     0x4        // 系统文件
#define FILE_ATTR_LABEL      0x8        // 卷标
#define FILE_ATTR_DIR        0x10       // 目录
#define FILE_ATTR_ARCHIVE    0x20       // 归档

// MBR BIOS参数块
typedef struct _packed {
    u8 Jmp_short[3];            // 跳转指令
    u8 BPB_OEMName[8];          // OEM名称
    u16 BPB_BytesPerSec;        // 每扇区字节数
    u8 BPB_SecPerClus;          // 每簇扇区数
    u16 BPB_RsvdSecCnt;         // 保留扇区数
    u8 BPB_NumFATs;             // FAT表个数
    u16 BPB_RootEntCnt;         // 根目录项数
    u16 BPB_TotSec16;           // 总扇区数
    u8 BPB_Media;               // 媒体描述符
    u16 BPB_FatSz16;            // 每FAT扇区数
    u16 BPB_SecPerTrk;          // 每磁道扇区数 
    u16 BPB_NumHeads;           // 磁头数
    u32 BPB_HiddSec;            // 隐藏扇区数
    u32 BPB_TotSec32;           // 总扇区数
    u8 BS_drvNum;               // 驱动器号
    u8 BS_Reserved1;            // 保留
    u8 BS_BootSig;              // 扩展引导标志
    u32 BS_VolID;               // 卷序列号
    u8 BS_VolLab[11];           // 卷标
    u8 BS_FileSysType[8];       // 文件系统类型
} mbr_bios_block_t;

// DBR参数块
typedef struct _packed {
    u8 Jmp_short[3];            // 跳转指令
    u8 BPB_OEMName[8];          // OEM名称
    u16 BPB_BytesPerSec;        // 每扇区字节数
    u8 BPB_SecPerClus;          // 每簇扇区数
    u16 BPB_RsvdSecCnt;         // 保留扇区数
    u8 BPB_NumFATs;             // FAT表个数
    u16 BPB_RootEntCnt;         // 根目录项数
    u16 BPB_TotSec16;           // 总扇区数
    u8 BPB_Media;               // 媒体描述符
    u16 BPB_FatSz16;            // 每FAT扇区数
    u16 BPB_SecPerTrk;          // 每磁道扇区数 
    u16 BPB_NumHeads;           // 磁头数
    u32 BPB_HiddSec;            // 隐藏扇区数
    u32 BPB_TotSec32;           // 总扇区数
    u8 BS_drvNum;               // 驱动器号
    u8 BS_Reserved1;            // 保留
    u8 BS_BootSig;              // 扩展引导标志
    u32 BS_VolID;               // 卷序列号
    u8 BS_VolLab[11];           // 卷标
    u8 BS_FileSysType[8];       // 文件系统类型
} dbr_block_t;

// FAT16分区信息
typedef struct _packed {
    u32 partition_start;        // 分区起始扇区
    u32 partition_sectors;      // 分区的总扇区数

    u32 dbr_start;              // DBR 起始扇区
    u32 dbr_sectors;            // DBR的扇区数

    u32 reserved_sectors;       // 保留扇区数

    u32 fat_start;              // FAT1 起始扇区
    u32 fat_sectors;            // FAT表的扇区数
    u32 fat_count;              // FAT表个数

    u32 rootdir_start;          // 根目录起始扇区
    u32 rootdir_sectors;        // 根目录占用的扇区数

    u32 data_start;             // 数据区起始扇区
    u32 data_sectors;           // 数据区总扇区数
    u32 data_balance;           // 数据区簇号平衡值
} partition_info_t;

// FAT16分区表项
typedef struct {
    u8  boot_indicator;         // 启动标志 (0x80=活动分区, 0x00=非活动分区)
    u8  chs_start[3];           // 起始 CHS (柱面、磁头、扇区)
    u8  partition_type;         // 分区类型码 (0x06=FAT16(LBA))
    u8  chs_end[3];             // 结束 CHS
    u32 start_lba;              // 起始 LBA (小端存储)
    u32 sector_count;           // 总扇区数 (小端存储)
} partition_entry_t;

// FAT16目录项
typedef struct _packed {
    u8 DIR_Name[8];             // 文件名
    u8 DIR_Ext[3];              // 扩展名
    u8 DIR_Attr;                // 属性
    u8 reserved[10];            // 保留
    u16 DIR_WriteTime;          // 写入时间
    u16 DIR_WriteDate;          // 写入日期
    u16 DIR_FstClus;            // 起始簇号
    u32 DIR_FileSize;           // 文件大小
} dir_t;

void read_fat(FILE *infile, u32 fat_num, void *fat_buffer);
void write_fat(FILE *infile, u32 fat_num, void *fat_buffer);

void read_rootdir(FILE *infile, void *rootdir_buffer);
void write_rootdir(FILE *infile, void *rootdir_buffer);

void read_mbr(FILE *infile, void *mbr_buffer);
void read_dbr(FILE *infile, void *dbr_buffer);

void read_partition(void *mbr_buffer);
void print_partition_info(partition_info_t *partition_info);
void get_partition_info(FILE *infile, u32 partition_num, partition_info_t *partition_info, mbr_bios_block_t *mbr_bios_block);

void get_file_name(char *file_name, char *input_file_name);
void convert_file_name(char *fat16_name, char *input_file_name);

u16 get_cluster(u16 *fat_buffer);
u16 link_cluster(u16 *fat_buffer, u16 cluster);

dir_t *get_dir_entry(dir_t *rootdir_buffer);

void write_file_to_fat16(FILE *infile, FILE *outfile, u32 partition_num, char *input_file_name);
void print_partition_table(FILE *infile, mbr_bios_block_t *mbr_bios_block);

#endif