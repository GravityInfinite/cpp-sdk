

#ifndef CPP_SERVER_SDK_GE_BATCH_CONSUMER_H
#define CPP_SERVER_SDK_GE_BATCH_CONSUMER_H
#include "GEConsumer.h"
#include <vector>
#include "GENetwork.h"
#include <mutex>

namespace GEData {

    class GEBatchConsumer : public GEConsumer {
    public:
        /**
         * Construct
         * @param serverUrl The server url of your project
         * @param batchSize buffer size
         */
        GEBatchConsumer(const std::string& serverUrl, int32_t batchSize = 20);
        ~GEBatchConsumer() override;
        void add(GEJsonObject obj) override;
        void flush() override;
        void close() override;

    private:
        std::string m_serverUrl;
        int32_t m_batchSize;
        std::vector<GEJsonObject> m_dataList;
        GENetwork m_network;
        std::mutex m_mutex;
        void sendData();
    };
}

#endif //CPP_SERVER_SDK_GE_BATCH_CONSUMER_H
