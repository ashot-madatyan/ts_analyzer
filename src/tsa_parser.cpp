/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_parser.cpp
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#include "tsa_parser.h"
#include "tsa_tspacket.h"
#include "tsa_pat.h"
#include "tsa_pmt.h"
#include "tsa_buffer.h"
#include "tsa_logger.h"
#include "tsa_tsmonitor.h"

ts_parser::ts_parser()
    : total_packets_count_(0)
    , current_timestamp_(0)
{
    pat_found_     = false;
    pmt_found_     = false;
    exp_pat_pid_   = 0x00000000;
    exp_pmt_pid_   = 0xFFFFFFFF;
    exp_video_pid_ = 0xFFFFFFFF;
}

ts_parser::~ts_parser()
{
}
void ts_parser::register_monitor(ts_monitor_base* pmon)
{
    monitors_.push_back(pmon);
}

/** Returns true iff video is an MPEG1/2/3, H264 or open cable video stream. */
bool ts_parser::is_video(unsigned int type)
{
    return ((MPEG1Video == type) ||
            (MPEG2Video == type) ||
            (MPEG4Video == type) ||
            (H264Video == type) ||
            (VC1Video == type) ||
            (OpenCableVideo == type));
}

void ts_parser::on_buffer(buffer_entry* pbuffer)
{
    process_data(pbuffer);
}

int ts_parser::process_data(buffer_entry* pentry)
{
    int pkt_count = pentry->length() / TS_PKT_SIZE;
    int rem_count = pentry->length() % TS_PKT_SIZE;
    int retv = 0;

    if (rem_count)
    {
        LOG_ERROR("buffer entry length not multiple of %d", TS_PKT_SIZE);
        return -1;
    }

    char* pbuffer = pentry->buffer();
    current_timestamp_ = pentry->timestamp();

    for (int i = 0; i < pkt_count; ++i)
    {
        // Process each individual packet data
        ts_packet pkt(total_packets_count_);
        pkt.store_raw_buffer((uint8_t*)pbuffer);
        pkt.parse();

        // A little trick to make the packet timestamps different from each other
        pkt.set_timestamp(current_timestamp_ + i);

        uint32_t pid = pkt.get_packet_pid();

        if (pid == exp_pat_pid_)
            retv = on_pat_packet(&pkt);
        else if (pid == exp_pmt_pid_)
            retv = on_pmt_packet(&pkt);
        else if (pid == exp_video_pid_)
            retv = on_video_packet(&pkt);
        else if (is_allowed_pid(pid))
            on_allowed_packet(&pkt);    
        else
            retv = on_unindentified_packet(&pkt);
        
        if (retv)
            return retv;

        pbuffer += TS_PKT_SIZE;
        ++total_packets_count_;
    }
    return 0;
}

void ts_parser::notify_listeners(ts_packet* pkt, void* p, pid_type pt)
{
    for (int i = 0; i < (int)monitors_.size(); ++i)
    {
        ts_monitor_base* pmon = monitors_[i];

        switch (pt)
        {
        case PIDT_PAT:
        {
            pat_packet* ppat = static_cast<pat_packet*>(p);
            pmon->on_pat_packet(pkt, ppat);
            break;
        }
        case PIDT_PMT:
        {
            pmt_packet* ppmt = static_cast<pmt_packet*>(p);
            pmon->on_pmt_packet(pkt, ppmt);
            break;
        }
        case PIDT_VIDEO:
        {
            pmon->on_video_packet(pkt);
            break;
        }
        case PIDT_AUDIO:
        {
            pmon->on_packet(pkt);
            break;
        }
        case PIDT_UNDEFINED:
        {
            pmon->on_packet(pkt);
            break;
        }

        default:
            break;
        }
    }
}

int ts_parser::on_pat_packet(ts_packet* pkt)
{
    pat_packet pat(0);
    pkt->set_type(PIDT_PAT);

    uint8_t* buf = (uint8_t*) pkt->data_buffer();
    pat.parse(buf + 4);
    exp_pmt_pid_ = pat.desc_.sdecs[0].prog_pid;

    if (false == pat_found_)
        pat_found_ = true;

    notify_listeners(pkt, &pat, PIDT_PAT);
    return 0;
}

int ts_parser::on_pmt_packet(ts_packet* pkt)
{
    pkt->set_type(PIDT_PMT);
    pmt_packet pmt(exp_pmt_pid_);
    uint8_t* buf = (uint8_t*) pkt->data_buffer();
    pmt.parse(buf + 4);
    pmt.get_cat_pids(allowed_pids_);
    pmt_desc desc = pmt.get_desc();
    
    for (int i = 0; i < pmt.get_stream_desc_count(); i++)
    {
        unsigned int streamtype = desc.sdecs[i].stream_type;
        if (is_video(streamtype))
        {
            exp_video_pid_ = desc.sdecs[i].el_pid;
            break;
        }
    }

    if (false == pmt_found_)
        pmt_found_ = true;

    notify_listeners(pkt, &pmt, PIDT_PMT);
    return 0;
}

int ts_parser::on_video_packet(ts_packet* pkt)
{
    pkt->set_type(PIDT_VIDEO);
    process_video_packet(pkt);
    
    notify_listeners(pkt, nullptr, PIDT_VIDEO);
    return 0;
}

int ts_parser::on_allowed_packet(ts_packet* pkt)
{
    notify_listeners(pkt, nullptr, PIDT_UNDEFINED);
    return 0;
}

int ts_parser::on_unindentified_packet(ts_packet* pkt)
{
    notify_listeners(pkt, nullptr, PIDT_UNDEFINED);
    return 0;
}

void ts_parser::process_video_packet(ts_packet* pkt)
{
#if 0   // IFrame start condition (when both PUSI and RAI are set)
    uint64_t pkt_num, pkt_pid;
    bool pusi = false, rai = false;

    pkt_num = pkt->get_packet_number();
    pkt_pid = pkt->get_packet_pid();
    pusi = pkt->pusi();
    rai = pkt->rai();

    // Test print the starts of the frames
    if (pusi && rai)
        printf("PUSI+RAI: num: %-10lld pcr: %lld\n", pkt->get_packet_number(), pkt->get_pcr());
#endif
}

bool ts_parser::is_allowed_pid(const uint32_t& pid)
{
    if (allowed_pids_.find(pid) != allowed_pids_.end())
        return true;
    return false;
}
