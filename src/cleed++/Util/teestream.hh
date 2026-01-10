#ifndef TEESTREAM_HH
#define TEESTREAM_HH

#include "teebuf.hh"

class teestream : public std::ostream
{
public:
    // Construct an ostream which tees output to the supplied
    // ostreams.
    teestream(std::ostream & o1, std::ostream & o2);
private:
    teebuf tbuf;
};

#endif /* TEESTREAM_HH */