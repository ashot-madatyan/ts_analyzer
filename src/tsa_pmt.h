/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_pmt.h
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_PMT_HEADER
#define TSA_PMT_HEADER

#include <set>

typedef unsigned char u8;
typedef unsigned short us16;

using std::set;

struct pmt_stream_descriptor
{
    u8      stream_type;               // 8 bits; 
    u8      rsvd_1;                    // 3 bits;
    us16    el_pid;                    // 13 bits;
    u8      rsvd_2;                    // 4 bits;
    us16    es_info_len;               // 2+10 bits;
    u8      es_raw_descriptor[256];    // actual size in @es_info_len
};

/**  Program Map Table descriptor */
struct pmt_desc 
{
    u8      ptr_field;                 // 8 bits; generally 0x00 for PMT
    u8      table_id;                  // 8 bits; always 0x02 for PMT
    u8      sec_sntx;                  // 1 bit;
    u8      always_zero;               // 1 bits;
    u8      rsvd_1;                    // 2 bits; always set to binary '11'
    us16    section_length;            // 2+10 bits;
    us16    prog_num;                  // 16 bits;
    u8      rsvd_2;                    // 2 bits;
    u8      version_num;               // 5 bits;
    u8      cur_next;                  // 1 bits;
    u8      sec_num;                   // 8 bits; always 0x00
    u8      last_sec_num;              // 8 bits; always 0x00
    u8      rsvd_3;                    // 3 bits;
    us16    pcr_pid;                   // 16 bits; PID of general timecode stream, or 0x1FFF
    u8      rsvd_4;                    // 4 bits;
    us16    prog_info_len;             // 2+10 bits; Sum size of following program descriptors. First two bits must be zero. 
    pmt_stream_descriptor sdecs[16];   // 
    us16    crc32;                     // 32 bits; 
};

#pragma pack(1)
struct program_descriptor
{
    unsigned int tag        : 8;
    unsigned int length     : 8;
} ;

struct cat_descriptor
{
    unsigned int ca_system_id;  // : 16;
    unsigned int reserved;      // : 3;
    unsigned int ca_pid;        // : 13;
    char private_data[32];

    cat_descriptor()
    {
        memset(this, 0, sizeof(cat_descriptor));
    }
};

#pragma pack()

static void read_cat_descriptor(u8* data, cat_descriptor& desc, program_descriptor* prog_desc)
{
    desc.ca_system_id = (data[0] << 8 ) | (data[1]);
    desc.reserved = (data[2] >> 4) & 0x07;
    desc.ca_pid = (data[2] & 0x1f) << 8 | data[3];

    for (unsigned int i = 0; i < (unsigned int) (prog_desc->length - 4); i++)
    {
        desc.private_data[i] = data[4 + i];
    }
}

class pmt_packet
{
public:

    pmt_packet(unsigned int pid) : pid_(pid)
    {
        memset(&desc_, 0, sizeof(desc_)); 
    };

    ~pmt_packet()
    {
    };

    us16 get_prog_num() const
    { 
        return desc_.prog_num;
    }

    /**
    *   On error return <= 0, otherwise return the count of bytes 
    *   that were successfully parsed.
    */
    int parse(u8* data)
    {
        int ret = 0;
        
        desc_.ptr_field = data[0];
        if (desc_.ptr_field != 0)
            data = data + desc_.ptr_field + 1;

        ret += 1;

        desc_.table_id = data[1];
        ret += 1;

        desc_.sec_sntx = (data[2] & 0x80) >> 7;
        desc_.always_zero = (data[2] & 0x40) >> 6;
        desc_.rsvd_1 = (data[2] & 0x30 ) >> 4;
        desc_.section_length = (data[2] & 0x0F) << 8;
        desc_.section_length |= data[3];
        u8* d1 = std::min(&data[3 + desc_.section_length - 4], data + 184);

        desc_.prog_num = data[4] << 8 | data[5];
        
        desc_.rsvd_2 = (data[6] & 0xC0) >> 6;
        desc_.version_num = (data[6] & 0xE) >> 1;
        desc_.cur_next = data[6] & 0x01;
        ret += 1;
        
        desc_.sec_num = data[7];
        desc_.last_sec_num = data[8];
        ret += 2;

        desc_.rsvd_3 = (data[9] & 0xE0) >> 5;
        desc_.pcr_pid = (data[9] & 0x1F) << 8;
        desc_.pcr_pid |= data[10];
        ret += 2;

        desc_.rsvd_4 = (data[11] & 0xF0) >> 4;

        desc_.prog_info_len = (data[11] & 0xF) << 8;
        desc_.prog_info_len |= data[12];
        
        // Read the program descriptor(s)
        u8* prog_data = &data[13];
        read_program_info(prog_data, desc_.prog_info_len);

        u8* d = &data[12 + desc_.prog_info_len + 1];
        int des_count = 0;
        while(d < d1) 
        {
            pmt_stream_descriptor* curdesc = &desc_.sdecs[des_count];
            curdesc->stream_type = d[0];
            curdesc->el_pid = (d[1] & 0x1f) << 8;
            curdesc->el_pid |= d[2];
            curdesc->es_info_len = (d[3] & 0x0F) << 8;
            curdesc->es_info_len |= d[4];

            d += (5 + curdesc->es_info_len);
            des_count++;
        }

        stream_desc_count_ = des_count;

        // Read the CRC
        desc_.crc32 = (d[0] << 24) | (d[1] << 16) | (d[2] << 8) | d[3];
        return ret;
    }

    int get_stream_desc_count() const
    {
        return stream_desc_count_; 
    }
    
    pmt_desc get_desc() const 
    {
        return desc_; 
    }

    int get_cat_pids(set<uint32_t>& pids)
    {
        int retv = cat_pids_.size();
        for (set<uint32_t>::iterator it = cat_pids_.begin(); it != cat_pids_.end(); it++)
        {
            pids.insert(*it);
        }

        return retv;
    }

protected:
    
    void read_program_info(u8* data, int prog_info_len)
    {
        if (0 == prog_info_len)
            return;

        u8* data_end = data + prog_info_len;
        while(data < data_end)
        {
            program_descriptor* progdesc = (program_descriptor*)data;

            if (progdesc->tag == 0x09)
            {
                cat_descriptor cat_desc;
                data += 2;
                read_cat_descriptor(data, cat_desc, progdesc);
                add_cat_pid(cat_desc.ca_pid);
            }
            
            // Skip to the next descriptor
            data += progdesc->length;            
        }
    }

    void add_cat_pid(const uint32_t& catpid)
    {
        cat_pids_.insert(catpid);
    }

protected:
    pmt_desc        desc_;
    unsigned int    pid_;
    int             stream_desc_count_;
    set<uint32_t>   cat_pids_;
};

#endif // TSA_PMT_HEADER
