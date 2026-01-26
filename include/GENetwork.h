

#ifndef CPP_SERVER_SDK_GE_NETWORK_H
#define CPP_SERVER_SDK_GE_NETWORK_H
#include "GEUtils.h"
#include <iostream>

namespace GEData {

    class GENetwork {
    public:
        GENetwork();
        ~GENetwork();
        static int64_t post(const std::string &url,GEJsonObject obj, std::string &strResponse);
        static int64_t debug_post(const std::string &url,GEJsonObject obj, std::string &strResponse);
    };
}

#endif //CPP_SERVER_SDK_GE_NETWORK_H
