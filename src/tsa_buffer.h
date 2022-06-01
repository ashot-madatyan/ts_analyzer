/////////////////////////////////////////////////////////////////////////////
// Name:        tsa_buffer.h
// Author:      Ashot Madatyan
// Created:     31-05-2022
/////////////////////////////////////////////////////////////////////////////

#ifndef TSA_BUFFERS_HEADER
#define TSA_BUFFERS_HEADER

#include <stdint.h>
#include <list>
#include <vector>

using std::list;
using std::vector;

static const int BUFFER_ALLOC_SIZE = 1500;

class buffer_entry
{
public:
    buffer_entry() 
        : length_(0)
        , timestamp_(0)
    {}

    ~buffer_entry()    {}
    
    void                set_timestamp (const uint64_t& tmstmp) { timestamp_ = tmstmp; }
    const uint64_t&     timestamp() const { return timestamp_; }
    void                set_length(const int& length) { length_ = length; }
    const int&          length() const { return length_; }
    char*               buffer() { return buffer_; }

    /** Return the capacity of the buffer. */
    int                 size() { return BUFFER_ALLOC_SIZE; }
    
protected:
    int         length_;
    uint64_t    timestamp_;
    char        buffer_[BUFFER_ALLOC_SIZE];
};

#endif // TSA_BUFFERS_HEADER
