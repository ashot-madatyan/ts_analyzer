/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_null_audio.cpp
// Author:      Ashot Madatyan
// Created:     06-06-2022
/////////////////////////////////////////////////////////////////////////////

#include "tsa_null_audio.h"
#include "tsa_pat.h"
#include "tsa_pmt.h"
#include "tsa_tspacket.h"

ts_null_audio::ts_null_audio() :
	pfout_(nullptr)
{}

ts_null_audio::~ts_null_audio() 
{
	if (pfout_)
		fclose(pfout_);
}

void ts_null_audio::init(const char* pfname)
{
	pfout_ = fopen(pfname, "wb");
}

void ts_null_audio::set_audio_pids(vector<int> pids)
{
	for (int k : pids)
		aud_.audio_pids.insert(k);
}

void ts_null_audio::on_pat_packet(ts_packet* ppkt, pat_packet* ppat)
{
	uint32_t pid = ppkt->get_packet_pid();
	if (pfout_)
		fwrite(ppkt->get_raw_buffer(), 1, TS_PKT_SIZE, pfout_);	
}

void ts_null_audio::on_pmt_packet(ts_packet* ppkt, pmt_packet* ppmt)
{
	uint32_t pid = ppkt->get_packet_pid();
	if (pfout_)
		fwrite(ppkt->get_raw_buffer(), 1, TS_PKT_SIZE, pfout_);

}

void ts_null_audio::on_video_packet(ts_packet* ppkt)
{
	//uint32_t pid = ppkt->get_packet_pid();
	on_packet(ppkt);
	//if (pfout_)
	//	fwrite(ppkt->get_raw_buffer(), 1, TS_PKT_SIZE, pfout_);
}

void ts_null_audio::on_packet(ts_packet* ppkt)
{
	uint32_t pid = ppkt->get_packet_pid();
	char* pout = (char*) ppkt->get_raw_buffer();

	if (aud_.audio_pids.find(pid) != aud_.audio_pids.end())
		pout = aud_.null_packet;

	if (pfout_)
		fwrite(pout, 1, TS_PKT_SIZE, pfout_);
}

