#include "GEDebugConsumer.h"
#include "GEUtils.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <utility>

namespace GEData {

    GEDebugConsumer::GEDebugConsumer( const std::string& serverUrl){
        this->m_serverUrl = serverUrl ;
        GELog::info(  " serverUrl: " + m_serverUrl  );
    }

    GEDebugConsumer::~GEDebugConsumer(void) = default;

    void GEDebugConsumer::add(GEJsonObject obj) {
        m_mutex.lock();

        sendData(obj);

        m_mutex.unlock();
    }

    void GEDebugConsumer::flush() {
         GELog::info("flush");
    }

    void GEDebugConsumer::close() {
        flush();
        GELog::info("close");
    }

    void GEDebugConsumer::sendData(GEJsonObject obj) {
        std::string strResponse;
        int64_t code = GEData::GENetwork::debug_post(m_serverUrl, obj, strResponse);

        std::string readdata = strResponse;

        rapidjson::Document document;
        document.Parse<0>(readdata.c_str());

        if (document.HasMember("code")) {
            rapidjson::Value& node1 = document["code"];
             if (node1.IsInt() ) {
                if (node1!=0){
                  GELog::info("Send data: fail");
                }else{
                  GELog::info("Send data: succeed");
                }
            }
        }
    }
}