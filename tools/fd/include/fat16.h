#ifndef __FAT16_H__
#define __FAT16_H__

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MBR_PARTITION_COUNT 4           // MBR�����������
#define PARTITION_SIZE  16              // MBR���������С
#define PARTITION_OFFSET 0x1BE          // MBR������ƫ��

#define FILE_ATTR_RW         0x0        // ��д
#define FILE_ATTR_READ_ONLY  0x1        // ֻ��
#define FILE_ATTR_HIDDEN     0x2        // ����
#define FILE_ATTR_SYSTEM     0x4        // ϵͳ�ļ�
#define FILE_ATTR_LABEL      0x8        // ���
#define FILE_ATTR_DIR        0x10       // Ŀ¼
#define FILE_ATTR_ARCHIVE    0x20       // �鵵

// MBR BIOS������
typedef struct _packed {
    u8 Jmp_short[3];            // ��תָ��
    u8 BPB_OEMName[8];          // OEM����
    u16 BPB_BytesPerSec;        // ÿ�����ֽ���
    u8 BPB_SecPerClus;          // ÿ��������
    u16 BPB_RsvdSecCnt;         // ����������
    u8 BPB_NumFATs;             // FAT�����
    u16 BPB_RootEntCnt;         // ��Ŀ¼����
    u16 BPB_TotSec16;           // ��������
    u8 BPB_Media;               // ý��������
    u16 BPB_FatSz16;            // ÿFAT������
    u16 BPB_SecPerTrk;          // ÿ�ŵ������� 
    u16 BPB_NumHeads;           // ��ͷ��
    u32 BPB_HiddSec;            // ����������
    u32 BPB_TotSec32;           // ��������
    u8 BS_drvNum;               // ��������
    u8 BS_Reserved1;            // ����
    u8 BS_BootSig;              // ��չ������־
    u32 BS_VolID;               // �����к�
    u8 BS_VolLab[11];           // ���
    u8 BS_FileSysType[8];       // �ļ�ϵͳ����
} mbr_bios_block_t;

// DBR������
typedef struct _packed {
    u8 Jmp_short[3];            // ��תָ��
    u8 BPB_OEMName[8];          // OEM����
    u16 BPB_BytesPerSec;        // ÿ�����ֽ���
    u8 BPB_SecPerClus;          // ÿ��������
    u16 BPB_RsvdSecCnt;         // ����������
    u8 BPB_NumFATs;             // FAT�����
    u16 BPB_RootEntCnt;         // ��Ŀ¼����
    u16 BPB_TotSec16;           // ��������
    u8 BPB_Media;               // ý��������
    u16 BPB_FatSz16;            // ÿFAT������
    u16 BPB_SecPerTrk;          // ÿ�ŵ������� 
    u16 BPB_NumHeads;           // ��ͷ��
    u32 BPB_HiddSec;            // ����������
    u32 BPB_TotSec32;           // ��������
    u8 BS_drvNum;               // ��������
    u8 BS_Reserved1;            // ����
    u8 BS_BootSig;              // ��չ������־
    u32 BS_VolID;               // �����к�
    u8 BS_VolLab[11];           // ���
    u8 BS_FileSysType[8];       // �ļ�ϵͳ����
} dbr_block_t;

// FAT16������Ϣ
typedef struct _packed {
    u32 partition_start;        // ������ʼ����
    u32 partition_sectors;      // ��������������

    u32 dbr_start;              // DBR ��ʼ����
    u32 dbr_sectors;            // DBR��������

    u32 reserved_sectors;       // ����������

    u32 fat_start;              // FAT1 ��ʼ����
    u32 fat_sectors;            // FAT���������
    u32 fat_count;              // FAT�����

    u32 rootdir_start;          // ��Ŀ¼��ʼ����
    u32 rootdir_sectors;        // ��Ŀ¼ռ�õ�������

    u32 data_start;             // ��������ʼ����
    u32 data_sectors;           // ��������������
    u32 data_balance;           // �������غ�ƽ��ֵ
} partition_info_t;

// FAT16��������
typedef struct {
    u8  boot_indicator;         // ������־ (0x80=�����, 0x00=�ǻ����)
    u8  chs_start[3];           // ��ʼ CHS (���桢��ͷ������)
    u8  partition_type;         // ���������� (0x06=FAT16(LBA))
    u8  chs_end[3];             // ���� CHS
    u32 start_lba;              // ��ʼ LBA (С�˴洢)
    u32 sector_count;           // �������� (С�˴洢)
} partition_entry_t;

// FAT16Ŀ¼��
typedef struct _packed {
    u8 DIR_Name[8];             // �ļ���
    u8 DIR_Ext[3];              // ��չ��
    u8 DIR_Attr;                // ����
    u8 reserved[10];            // ����
    u16 DIR_WriteTime;          // д��ʱ��
    u16 DIR_WriteDate;          // д������
    u16 DIR_FstClus;            // ��ʼ�غ�
    u32 DIR_FileSize;           // �ļ���С
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