
#include "GEBatchConsumer.h"
#include "GEUtils.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <utility>

namespace GEData {

    GEBatchConsumer::GEBatchConsumer(const std::string& serverUrl, int32_t batchSize):
       m_batchSize(batchSize)
    {
        m_serverUrl = serverUrl;
        GELog::info("serverUrl: " + m_serverUrl);
    }

    GEBatchConsumer::~GEBatchConsumer(void) = default;


    void GEBatchConsumer::add(GEJsonObject obj) {
        m_mutex.lock();

        m_dataList.emplace_back(obj);

        GELog::info("Enqueue data to buffer. Buffer count: " + std::to_string(m_dataList.size()));

        if (m_dataList.size() >= m_batchSize) {
            sendData();
        }

        m_mutex.unlock();
    }

    void GEBatchConsumer::flush() {
        GELog::info("GEBatchConsumer flush data.");
        m_mutex.lock();
        sendData();
        m_mutex.unlock();
    }

    void GEBatchConsumer::sendData() {

        std::string strResponse;
        size_t size = m_dataList.size();
        if (size <= 0) return;

        std::map<std::string, std::vector<GEJsonObject>> client_id_map;

        for (size_t i = 0; i < size; i++) {
            std::shared_ptr<GEJsonObject::GEValueNode> client_id_node = m_dataList[i].FindNode("client_id");
            std::string client_id = client_id_node->m_data_string;
            std::shared_ptr<GEJsonObject::GEValueNode> event_list_node = m_dataList[i].FindNode("event_list");

            // 根据event_list_node的类型来处理
            if (event_list_node->node_type_ == GEJsonObject::OBJECTS) {
                // 如果event_list是对象列表，则将每个对象添加到client_id_map
                for (const auto& event : event_list_node->list_obj_) {
                    client_id_map[client_id].push_back(event);
                }
            } else if (event_list_node->node_type_ == GEJsonObject::OBJECT) {
                // 如果event_list是单个对象，则直接添加
                client_id_map[client_id].push_back(event_list_node->object_data_);
            }
        }

        bool all_ok=true;

        for (auto& pair : client_id_map) {
            std::string client_id = pair.first;
            std::vector<GEJsonObject>& events = pair.second;

            GEJsonObject finalPropertiesDic;
            finalPropertiesDic.SetString("client_id", client_id);
            finalPropertiesDic.SetList("event_list", events);

            int64_t code = GEData::GENetwork::post(m_serverUrl, finalPropertiesDic, strResponse);

            GELog::info("Send data, response: " + strResponse);


            std::string readdata = strResponse;

            rapidjson::Document document;
            document.Parse<0>(readdata.c_str());

            if (document.HasMember("code")) {
                rapidjson::Value& node1 = document["code"];
                 if (node1.IsInt() ) {
                    if (node1!=0){
                      all_ok=false;
                      GELog::info("Send data: fail ");
                    }
                }
            }
        }

        if (all_ok){
             m_dataList.erase(m_dataList.begin(), m_dataList.begin() + (int64_t)(size));
        }
    }

    void GEBatchConsumer::close() {
        flush();
        GELog::info("GEBatchConsumer close.");
    }
}
