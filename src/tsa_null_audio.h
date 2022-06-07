/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_null_audio.h
// Author:      Ashot Madatyan
// Created:     06-06-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_NULL_AUDIO_HEADER
#define TSA_NULL_AUDIO_HEADER

#include "tsa_tsmonitor.h"
#include "tsa_common.h"
#include <cstring>
#include <cstdio>
#include <set>
#include <vector>

using std::set;
using std::vector;

struct audio_aux_data
{
	char null_packet[TS_PKT_SIZE];
	set<int> audio_pids;

	audio_aux_data()
	{
		memset(null_packet, 0xFF, TS_PKT_SIZE);
		null_packet[0] = 0x47;
		null_packet[1] = 0x1F;
	}
};

/**
 *   class ts_null_audio
 *
 *   This is a sample class that overwrites all the audio packets with a TS NULL packet (PID=8191)
 */
class ts_null_audio : public ts_monitor_base
{
public:
	ts_null_audio();
	virtual ~ts_null_audio();

	// Events
	virtual void on_pat_packet(ts_packet* ppkt, pat_packet* ppat);
	virtual void on_pmt_packet(ts_packet* ppkt, pmt_packet* ppmt);
	virtual void on_video_packet(ts_packet* ppkt);
	virtual void on_packet(ts_packet* ppkt);

	void init(const char* pfname);
	void set_audio_pids(vector<int>);

private:
	audio_aux_data aud_;
	FILE* pfout_;
};

#endif // TSA_NULL_AUDIO_HEADER
