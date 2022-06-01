/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_ts_info.cpp
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#include "tsa_ts_info.h"
#include "tsa_pat.h"
#include "tsa_pmt.h"

ts_info::ts_info() {}
ts_info::~ts_info() {}

void ts_info::on_pat_packet(ts_packet* ppkt, pat_packet* ppat)
{
	++stats_.pat_count;
	++stats_.total_count;
}

void ts_info::on_pmt_packet(ts_packet* ppkt, pmt_packet* ppmt)
{
	++stats_.pmt_count;
	++stats_.total_count;
}

void ts_info::on_video_packet(ts_packet* ppkt)
{
	++stats_.video_count;
	++stats_.total_count;
}

void ts_info::on_packet(ts_packet* ppkt)
{
	++stats_.total_count;
}

packet_stats ts_info::stats() const
{
	return stats_;
}
