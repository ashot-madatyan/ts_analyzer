/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_pat.h
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_PAT_HEADER
#define TSA_PAT_HEADER

#include <stdio.h>
#include <string>
#include <string.h>

typedef unsigned char u8;
typedef unsigned short us16;


struct pat_program_descriptor 
{
    u8 prog_num;                    // 16 bits; 
    u8 rsvd_1;                      // 3 bits; always set to binary '111'
    us16 prog_pid;                  // 13 bits; packets with this PID are assumed to be PMT tables
};

/**  Program Association Table descriptor */
struct pat_descriptor 
{
    u8 ptr_field;                       // 8 bits; Number of bytes to the start of payload
    u8 table_id;                        // 8 bits; always 0x00 for PAT
    u8 sec_sntx;                        // 1 bit; always 1 for PAT
    u8 always_zero;                     // 1 bits; always 0 for PAT
    u8  rsvd_1;                         // 2 bits; always set to binary '11'
    us16 section_length;                // 2+10 bits;
    us16 tr_stream_id;                  // 16 bits; user data, not important for demuxers
    u8  rsvd_2;                         // 2 bits;
    u8  version_num;                    // 5 bits; Table version number
    u8  cur_next;                       // 1 bits;
    u8  sec_num;                        // 8 bits; first is 0x00
    u8  last_sec_num;                   // 8 bits; index of last section of PAT table
    pat_program_descriptor sdecs[16];   // 
    us16 crc32;                         // 32 bits; 
};

class pat_packet 
{
public:

    pat_packet(unsigned int pid) : pid_(pid), num_progs_(0)
    {
        memset(&desc_, 0, sizeof(desc_)); 
    };

    ~pat_packet() { };

    /**
    *  On error return <= 0, otherwise return the count of bytes 
    *  that were successfully parsed.
    */
    int parse(u8* data)
    {
        int ret = 0;
        
        desc_.ptr_field = data[0];
        if (desc_.ptr_field != 0)
            data = data + desc_.ptr_field + 1;

        desc_.table_id = data[1];
        desc_.sec_sntx = (data[2] & 0x80) >> 7;
        desc_.always_zero = (data[2] & 0x40) >> 6;
        desc_.rsvd_1 = (data[2] & 0x30 ) >> 4;
        desc_.section_length = (data[2] & 0x0F) << 8;
        desc_.section_length |= data[3];

        desc_.tr_stream_id = data[4] << 8 | data[5];
        desc_.rsvd_2 = (data[6] & 0xC0) >> 6;
        desc_.version_num = (data[6] & 0xE) >> 1;
        desc_.cur_next = data[6] & 0x01;
        desc_.sec_num = data[7];
        desc_.last_sec_num = data[8];

        u8* d = &data[9];
        int des_count = 0;
        int calc_desc_count = (desc_.section_length - 9 ) / 4;
        num_progs_ = calc_desc_count;

        for(int i = 0; i < calc_desc_count; ++i) 
        {
            pat_program_descriptor* curdesc = &desc_.sdecs[des_count];
            curdesc->prog_num = (d[0] << 8) | d[1];
            curdesc->prog_pid = (d[2] & 0x1F) << 8;
            curdesc->prog_pid |= d[3];

            d += 4;
            des_count++;
        }

        return ret;
    }

    void dump()
    {
        printf("PAT PID: %u\n", pid_);
        printf("  TABLE_ID: %u\n", desc_.table_id);
        
        printf("  PROGRAMS\n");
        for (int i = 0; i < num_progs_; ++i){
            printf("    PROGRAM NUM: %-5d MAP_ID: %-5d\n", 
                    desc_.sdecs[i].prog_num,
                    desc_.sdecs[i].prog_pid);
        }
        printf("\n");
    }

public:
    pat_descriptor  desc_;
    unsigned int    pid_;
    int             num_progs_;
};

#endif // TSA_PAT_HEADER
