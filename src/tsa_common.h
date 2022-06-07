/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_common.h
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_COMMON_HEADER
#define TSA_COMMON_HEADER

#include <stdint.h>

static const int32_t        TS_PKT_SIZE                 = 188;
static const int32_t        TS_PKT_MTU_CNT              = 7;
static const int32_t        MTU_PKT_SIZE                = TS_PKT_SIZE * TS_PKT_MTU_CNT;
static const uint32_t       INVALID_VALUE               = 0xFFFFFFFF;
static const uint32_t       TSF_NAME_LENGTH             = 32;
static const uint32_t       NP_INDEX_HEADER_DATA_SIZE   = 128;

typedef uint64_t timestamp_t;

enum pid_type
{
    PIDT_UNDEFINED  = -1,
    PIDT_PAT        = 0,
    PIDT_PMT        = 1,
    PIDT_AUDIO      = 2,
    PIDT_VIDEO      = 3,
    PIDT_CAT        = 4,
};

/** 
 *   StreamID - Contains listing of PMT Stream ID's for various A/V Stream types.
 *
 *   This is used by the Program Map Table (PMT), and often used by other
 *   tables, such as the DVB SDT table.
 */
enum StreamID
{
    // video
    MPEG1Video = 0x01, // ISO 11172-2 (aka MPEG-1)
    MPEG2Video = 0x02, // ISO 13818-2 & ITU H.262 (aka MPEG-2)
    MPEG4Video = 0x10, // ISO 14492-2 (aka MPEG-4)
    H264Video = 0x1b, // ISO 14492-10 & ITU H.264 (aka MPEG-4-AVC)
    OpenCableVideo = 0x80,
    VC1Video = 0xea, // SMPTE 421M video codec (aka VC1) in Blu-Ray

    // audio
    MPEG1Audio = 0x03, // ISO 11172-3
    MPEG2Audio = 0x04, // ISO 13818-3
    MPEG2AACAudio = 0x0f, // ISO 13818-7 Audio w/ADTS syntax
    MPEG2AudioAmd1 = 0x11, // ISO 13818-3/AMD-1 Audio using LATM syntax
    AC3Audio = 0x81, // A/53 Part 3:2009 6.7.1
    EAC3Audio = 0x87, // A/53 Part 3:2009 6.7.3
    DTSAudio = 0x8a,

    // DSM-CC Object Carousel
    DSMCC = 0x08, // ISO 13818-1 Annex A DSM-CC & ITU H.222.0
    DSMCC_A = 0x0a, // ISO 13818-6 type A Multi-protocol Encap
    DSMCC_B = 0x0b, // ISO 13818-6 type B Std DSMCC Data
    DSMCC_C = 0x0c, // ISO 13818-6 type C NPT DSMCC Data
    DSMCC_D = 0x0d, // ISO 13818-6 type D Any DSMCC Data
    DSMCC_DL = 0x14, // ISO 13818-6 Download Protocol
    MetaDataPES = 0x15, // Meta data in PES packets
    MetaDataSec = 0x16, // Meta data in metadata_section's
    MetaDataDC = 0x17, // ISO 13818-6 Metadata in Data Carousel
    MetaDataOC = 0x18, // ISO 13818-6 Metadata in Object Carousel
    MetaDataDL = 0x19, // ISO 13818-6 Metadata in Download Protocol

    // other
    PrivSec = 0x05, // ISO 13818-1 private tables   & ITU H.222.0
    PrivData = 0x06, // ISO 13818-1 PES private data & ITU H.222.0

    MHEG = 0x07, // ISO 13522 MHEG
    H222_1 = 0x09, // ITU H.222.1

    MPEG2Aux = 0x0e, // ISO 13818-1 auxiliary & ITU H.222.0

    FlexMuxPES = 0x12, // ISO 14496-1 SL/FlexMux in PES packets
    FlexMuxSec = 0x13, // ISO 14496-1 SL/FlexMux in 14496_sections

    MPEG2IPMP = 0x1a, // ISO 13818-10 Digital Restrictions Mangment
    MPEG2IPMP2 = 0x7f, // ISO 13818-10 Digital Restrictions Mangment

    Splice = 0x86, // ANSI/SCTE 35 2007 

    // special id's, not actually ID's but can be used in FindPIDs
    AnyMask = 0xFFFF0000,
    AnyVideo = 0xFFFF0001,
    AnyAudio = 0xFFFF0002
};

#endif // TSA_COMMON_HEADER
