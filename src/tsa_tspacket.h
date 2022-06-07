/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_tspacket.h
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_PACKET_HEADER
#define TSA_PACKET_HEADER

#include <stdio.h>
#include <string>
#include <cstring>
#include "tsa_common.h"


using std::string;

#pragma pack(1)

typedef struct ts_packet_header_desc {
    uint32_t sync_byte           : 8; 
    uint32_t tr_err_ind          : 1;
    uint32_t pusi                : 1;
    uint32_t tr_priority         : 1;
    uint32_t pid                 : 13;
    uint32_t scramble_control    : 2;
    uint32_t adf_flag            : 2;
    uint32_t continuity_cntr     : 4;

    ts_packet_header_desc()
    {
        sync_byte            = 0;
        tr_err_ind            = 0;
        pusi                = 0;
        tr_priority         = 0;
        pid                 = 0;
        scramble_control    = 0;
        adf_flag            = 0;
        continuity_cntr     = 0;
    }
} ts_packet_header_desc_t;

struct ts_adaptation_field_descriptor 
{
    uint32_t length              : 8; 
    uint32_t discontinuity       : 1;
    uint32_t rnd_acc_ind         : 1;
    uint32_t es_prio_ind         : 1;
    uint32_t pcr_flag            : 1;
    uint32_t opcr_flag           : 1;
    uint32_t splicing_pt_flag    : 1;
    uint32_t tr_priv_data_flag   : 1;
    uint32_t af_ext_flag         : 1;
    uint64_t pcr;
    uint64_t opcr;
    uint32_t splice_cnt_down     : 8;

    ts_adaptation_field_descriptor()
    {
        length              = 0; 
        discontinuity       = 0;
        rnd_acc_ind         = 0;
        es_prio_ind         = 0;
        pcr_flag            = 0;
        opcr_flag           = 0;
        splicing_pt_flag    = 0;
        tr_priv_data_flag   = 0;
        af_ext_flag         = 0;
        pcr                 = 0;
        opcr                = 0;
        splice_cnt_down     = 0;
    }
};

#pragma pack()

class ts_packet;

class adaptation_field
{
    friend class ts_packet;

    adaptation_field() 
    {
    }

    ~adaptation_field() {}

    int parse(uint8_t* data, int bufsize)
    {
        int ret = 0;

        desc.length = data[0];
        desc.discontinuity = (data[1] & 0x80) >> 7;
        desc.rnd_acc_ind = (data[1] & 0x40) >> 6;
        desc.es_prio_ind = (data[1] & 0x20) >> 5;
        desc.pcr_flag = (data[1] & 0x10) >> 4;
        desc.opcr_flag = (data[1] & 0x08) >> 3;
        desc.splicing_pt_flag = (data[1] & 0x04) >> 2;
        desc.tr_priv_data_flag = (data[1] & 0x02) >> 1;
        desc.af_ext_flag = data[1] & 0x01;

        ret += 2;

        if (desc.pcr_flag){
            desc.pcr = _read_pcr(data + 2);
            if (desc.opcr_flag) 
            {
                desc.opcr = _read_pcr(data + 2 + 6);
            }
        }
        else if (desc.opcr_flag){
            desc.opcr = _read_pcr(data + 2 + 6);
        }

        ret = desc.length + 1;
        return ret;
    }
    
    string get_desc_string() const
    {
        string ret;
        char buf[1024];
        int bs = 0;
        
        bs = sprintf(buf, "RAI: %-1d PCR: %-9zu", desc.rnd_acc_ind, (uint32_t)desc.pcr);
        ret.assign(buf);
        return ret;
    }

    ts_adaptation_field_descriptor& get_descriptor()
    {
        return desc;
    }

    void set_descriptor(ts_adaptation_field_descriptor& desc)
    {
        this->desc = desc;
    }

protected:

    uint64_t _read_pcr(uint8_t* buf)
    {
        uint64_t pcr = 0;

        pcr = ((uint64_t)buf[0] << 25) | (buf[1] << 17) | (buf[2] << 9) | (buf[3] << 1) | (buf[4] >> 7);
        // Plus the program clock reference extension
        pcr = (pcr * 300) + ((buf[4] & 1) << 8) + buf[5];
        
        return pcr;
    }

protected:
    ts_adaptation_field_descriptor desc;
};

class ts_packet
{
public:
    
    ts_packet(uint32_t pkt_num = 0) : af_present_(false)
    {
        this->pkt_number_ = pkt_num;
        pkt_time_ = 0;
    }

    ~ts_packet() 
    {
    }

    ts_packet(const ts_packet& pkt)
    {
        if (this != &pkt)
        {
            hdr_         = pkt.hdr_;
            pkt_number_  = pkt.pkt_number_;
            af_          = pkt.af_;
            af_present_  = pkt.af_present_;

            memcpy(rawdata, pkt.rawdata, TS_PKT_SIZE);
        }
    }

    int parse(uint8_t* data, int bufsize)
    {
        int ret = 4;

        hdr_.sync_byte = data[0];
        hdr_.tr_err_ind = (data[1] & 0x80) >> 7;
        hdr_.pusi = (data[1] & 0x40) >> 6;
        hdr_.tr_priority = (data[1] & 0x20) >> 5;
        hdr_.pid = ((data[1] & 0x1F) << 8) | data[2];
        hdr_.scramble_control = (data[3] & 0xC0) >> 6;        
        hdr_.adf_flag = (data[3] & 0x30) >> 4;
        hdr_.continuity_cntr = data[3] & 0x0F;

        if (2 == hdr_.adf_flag || 3 == hdr_.adf_flag)
        {
            af_present_ = true;
            af_.parse(data + 4, 184);
        }

        return ret;
    }

    int parse()
    {
        uint8_t* data = (uint8_t*)&rawdata[0];
        int bufsize = TS_PKT_SIZE;
        return parse(data, bufsize);
    }

    string get_desc_string() const
    {
        string ret;
        char buf[1024];
        int bs = 0;

        bs = sprintf(buf, "TS Packet: %-9zu PID: %-4d PUSI: %-2d", (uint32_t) pkt_number_, hdr_.pid, hdr_.pusi);
        ret.assign(buf);

        string afs;
        if (af_present_)
            afs = af_.get_desc_string();

        ret = ret + " " + afs;
        return ret;
    }

    uint64_t get_packet_number() const 
    {
        return pkt_number_; 
    }
    
    uint32_t get_packet_pid() const 
    { 
        return hdr_.pid; 
    }

    uint64_t get_pcr() const 
    { 
        if (false == af_present_)
            return 0;

        return af_.desc.pcr;
    }
    
    bool pusi() const 
    {
        return hdr_.pusi;
    }
    
    bool rai() const 
    {
        if (!af_present_)
            return false;

        return  af_.desc.rnd_acc_ind;
    }

    void store_raw_buffer(uint8_t* buf)
    {
        memcpy(rawdata, buf, TS_PKT_SIZE);
    }

    void fetch_raw_buffer(uint8_t* buf)
    {
        memcpy(buf, rawdata, TS_PKT_SIZE);
    }

    int8_t* get_raw_buffer()
    {
        return &rawdata[0];
    }

    int8_t* data_buffer()
    {
        return &rawdata[0];
    }

    void set_type(const pid_type& tp)
    {
        //pkt_desc.set_type(tp);
    }
    
    void set_continuity_counter(unsigned int val)
    {
        uint8_t cc = val & 0x0F;
        cc = cc % 16; // since it goes from 0 - 15
        uint8_t* ccb = (uint8_t*) &rawdata[3];
        *ccb = *ccb & 0xF0; // clear the lower 4 bits
        *ccb = *ccb | cc;
    }

    unsigned int get_continuity_counter()
    {
        return hdr_.continuity_cntr;
    }

    void set_timestamp(const timestamp_t& ts)
    {
        pkt_time_ = ts;
    }
    
    timestamp_t get_timestamp()
    {
        return pkt_time_;
    }

protected:
    
    ts_packet_header_desc   hdr_;
    uint64_t                pkt_number_;
    adaptation_field        af_;
    int8_t                  rawdata[TS_PKT_SIZE];
    timestamp_t             pkt_time_;
    bool                    af_present_;
};

#endif // TSA_PACKET_HEADER
