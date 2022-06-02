/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_ts_info.h
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_TS_INFO_HEADER
#define TSA_TS_INFO_HEADER

#include "tsa_tsmonitor.h"
#include <map>

using std::map;

struct packet_stats
{
	int pat_count{ 0 };
	int pmt_count{ 0 };
	int video_count{ 0 };
	int total_count{ 0 };
	map<uint32_t, int> pids;
};

/**
 *   class ts_info
 *
 *   This is a sample class that collects and aggregates the packet information
 *   extracted from the transport stream (TS) wrapped Audio/Video and other packet types.
 */
class ts_info : public ts_monitor_base
{
public:
	ts_info();
	virtual ~ts_info();

	// Events
	virtual void on_pat_packet(ts_packet* ppkt, pat_packet* ppat);
	virtual void on_pmt_packet(ts_packet* ppkt, pmt_packet* ppmt);
	virtual void on_video_packet(ts_packet* ppkt);
	virtual void on_packet(ts_packet* ppkt);

	packet_stats stats() const;

private:
	// Count of packet of specific type
	packet_stats stats_;

};

#endif // TSA_TS_INFO_HEADER
