/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_logger.h
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_LOGGER_HEADER
#define TSA_LOGGER_HEADER

#include <stdio.h>

static const int LOG_DATA_BUFFER_SIZE = 256;

#if defined(WIN32)
#define LOG_MSG(log_prn_fmt, ...) do {                                    \
        char tmp_log_buf[LOG_DATA_BUFFER_SIZE];                                            \
        _snprintf(tmp_log_buf, LOG_DATA_BUFFER_SIZE, "MSG  : " log_prn_fmt, __VA_ARGS__);        \
        fprintf(stdout, "%s\n", tmp_log_buf); \
    } while(0);

#define LOG_INFO(log_prn_fmt, ...) do {                                    \
        char tmp_log_buf[LOG_DATA_BUFFER_SIZE];                                            \
        _snprintf(tmp_log_buf, LOG_DATA_BUFFER_SIZE, "INFO : " log_prn_fmt, __VA_ARGS__);        \
        fprintf(stdout, "%s\n", tmp_log_buf); \
    } while(0);

#define LOG_WARN(log_prn_fmt, ...) do {                                    \
        char tmp_log_buf[LOG_DATA_BUFFER_SIZE];                                            \
        _snprintf(tmp_log_buf, LOG_DATA_BUFFER_SIZE, "WARN : " log_prn_fmt, __VA_ARGS__);        \
        fprintf(stderr, "%s\n", tmp_log_buf); \
    } while(0);

#define LOG_ERROR(log_prn_fmt, ...) do {                                \
        char tmp_log_buf[LOG_DATA_BUFFER_SIZE];                                            \
        _snprintf(tmp_log_buf, LOG_DATA_BUFFER_SIZE, "ERROR: " log_prn_fmt, __VA_ARGS__);        \
        fprintf(stderr, "%s\n", tmp_log_buf); \
    } while(0);
#else

#define LOG_ERROR(log_prn_fmt, ...) do {                                \
        char tmp_log_buf[LOG_DATA_BUFFER_SIZE];                                            \
        snprintf(tmp_log_buf, LOG_DATA_BUFFER_SIZE, "ERROR: " log_prn_fmt, __VA_ARGS__);        \
        fprintf(stderr, "%s\n", tmp_log_buf); \
    } while(0);

#endif

#endif // TSA_LOGGER_HEADER
