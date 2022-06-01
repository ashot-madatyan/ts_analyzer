/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_tsmonitor.h
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_TS_MONITOR_HEADER
#define TSA_TS_MONITOR_HEADER

class ts_packet;
class pat_packet;
class pmt_packet;

class ts_monitor_base
{
public:

	ts_monitor_base()			{}
	virtual ~ts_monitor_base()	{}

	// Events
	virtual void on_pat_packet(ts_packet* ppkt, pat_packet* ppat) = 0;
	virtual void on_pmt_packet(ts_packet* ppkt, pmt_packet* ppmt) = 0;
	virtual void on_video_packet(ts_packet* ppkt) = 0;
	virtual void on_packet(ts_packet* ppkt) = 0;
};

#endif // TSA_TS_MONITOR_HEADER
