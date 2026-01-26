

#ifndef CPP_SERVER_SDK_GE_DEBUG_CONSUMER_H
#define CPP_SERVER_SDK_GE_DEBUG_CONSUMER_H
#include "GEConsumer.h"
#include <vector>
#include <mutex>
#include "GENetwork.h"

namespace GEData {

    class GEDebugConsumer : public GEConsumer {
    public:
        /**
         * Construct
         * @param serverUrl The server url of your project
         */
        GEDebugConsumer(const std::string& serverUrl);
        ~GEDebugConsumer() override;
        void add(GEJsonObject obj);
        void flush() override;
        void close() override;

    private:
        std::string m_serverUrl;
        GENetwork m_network;
        std::mutex m_mutex;
        void sendData(GEJsonObject obj);
    };
}

#endif //CPP_SERVER_SDK_GE_DEBUG_CONSUMER_H
