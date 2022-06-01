/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_parser.h
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_PARSER_HEADER
#define TSA_PARSER_HEADER

#include <set>
#include <vector>
#include "tsa_buffer.h"

using std::set;
using std::vector;

class tschunk;
class ts_packet;
class ts_monitor_base;

class ts_parser
{
public:
    ts_parser();
    ~ts_parser();

    void    register_monitor(ts_monitor_base*);
    void    on_buffer(buffer_entry* pbuffer);

protected:
    int     process_data(buffer_entry* pentry);
    void    process_video_packet(ts_packet* pkt);
    bool    is_allowed_pid(const uint32_t& pid);

    // Internal events triggered on parsing each individual TS packet
    int     on_pat_packet(ts_packet* pkt);
    int     on_pmt_packet(ts_packet* pkt);
    int     on_video_packet(ts_packet* pkt);
    int     on_allowed_packet(ts_packet* pkt);
    int     on_unindentified_packet(ts_packet* pkt);

private:
    int                         total_packets_count_;    // Total count of 188-byte packets processed up to now
    uint64_t                    current_timestamp_;
    uint32_t                    exp_pat_pid_;
    uint32_t                    exp_pmt_pid_;
    uint32_t                    exp_video_pid_;
    set<uint32_t>               allowed_pids_;
    bool                        pat_found_;        
    bool                        pmt_found_;        
    vector< ts_monitor_base*>   monitors_;        // List of parser monitoring objects
};

#endif // TSA_PARSER_HEADER
