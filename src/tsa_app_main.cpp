/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_app_main.cpp
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "tsa_logger.h"
#include "tsa_parser.h"
#include "tsa_common.h"
#include "tsa_ts_info.h"

void report(ts_info* ptsinfo)
{
	printf("============================\n");
	printf("STREAM STATISTICS\n----------------------------\n");
	printf("TOTAL PACKETS: %d\n", ptsinfo->stats().total_count);
	printf("PAT   PACKETS: %d\n", ptsinfo->stats().pat_count);
	printf("PMT   PACKETS: %d\n", ptsinfo->stats().pmt_count);
	printf("VIDEO PACKETS: %d\n", ptsinfo->stats().video_count);

	printf("\nPIDs in the TS\n----------------------------\n");
	// Print the PIDs stats
	for (auto k : ptsinfo->stats().pids)
	{
		printf("PID: %-5d %d\n", k, k.second);
	}

	printf("============================\n");
}

void process(const char* pfname)
{
	FILE* pf = fopen(pfname, "rb");
	
	ts_parser parser;
	ts_monitor_base* pmon = new ts_info;
	buffer_entry be;

	if (pf)
	{
		parser.register_monitor(pmon);
		int sz = 0;

		while ((sz = fread(be.buffer(), 1, MTU_PKT_SIZE, pf)) > 0)
		{
			if (sz % TS_PKT_SIZE != 0)
			{
				LOG_ERROR("Count of read data not a multiple of 188 (%d)", sz);
				return;
			}

			be.set_length(sz);
			parser.on_buffer(&be);
		}

		fclose(pf);
	}

	ts_info* ptsinfo = dynamic_cast<ts_info*>(pmon);
	report(ptsinfo);
	delete ptsinfo;
}

void usage()
{
	printf("Usage:\n");
	printf("tsanalyzer <input_ts_file>\n");
}

int main(int argc, const char* argv[])
{
	if (argc < 2)
	{
		usage();
		return -1;
	}

	process(argv[1]);
	return 0;
}
