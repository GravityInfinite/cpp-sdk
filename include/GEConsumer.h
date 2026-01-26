
#ifndef CPP_GE_CONSUMER_H
#define CPP_GE_CONSUMER_H
#include "GEUtils.h"
#include <ostream>

namespace GEData {
    class GEConsumer {

    public:
        virtual ~GEConsumer();

        virtual void add(GEJsonObject obj)=0;

        virtual void flush() = 0;

        virtual void close() = 0;
    };
}

#endif