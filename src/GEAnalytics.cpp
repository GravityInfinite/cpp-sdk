#include "GEAnalytics.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <chrono>

const static std::string TRACK                      = "track";
const static std::string PROFILE                      = "profile";

const static std::string USER_SET                   = "profile_set";
const static std::string USER_SET_ONCE              = "profile_set_once";
const static std::string USER_UNSET                 = "profile_unset";
const static std::string USER_DEL                   = "profile_delete";
const static std::string USER_INCREMENT             = "profile_increment";
const static std::string USER_MAX             = "profile_number_max";
const static std::string USER_MIN             = "profile_number_min";
const static std::string USER_APPEND                = "profile_append";
const static std::string USER_UNIQ_APPEND           = "profile_uniq_append";
const static std::string LIB_VERSION = "1.0.0";
const static std::string LIB_NAME = "C++";

namespace GEData {

//    GEAnalytics::GEAnalytics(GEConsumer &consumer): GEAnalytics(consumer) {
//
//    }

    GEAnalytics::GEAnalytics(GEConsumer &consumer) : m_consumer(consumer) {
        GELog::info("SDK initialization successful");
    }

    void GEAnalytics::track(const std::string& clientId, const std::string& eventName, GEPropertiesNode& properties) {
        GEJsonObject allProperties;
        allProperties.MergeFrom(properties);
        add(clientId, TRACK, eventName,  allProperties);
    }

    void GEAnalytics::flush() {
        GELog::info("SDK flush.");
        m_consumer.flush();
    }


    void GEAnalytics::user_set(const std::string& clientId, GEPropertiesNode& properties) {
        add(clientId, PROFILE, USER_SET, properties);
    }

    void GEAnalytics::user_set_once(const std::string& clientId, GEPropertiesNode& properties) {
        add(clientId, PROFILE, USER_SET_ONCE, properties);
    }

    void GEAnalytics::user_increment(const std::string& clientId, GEPropertiesNode& properties) {
        add(clientId, PROFILE, USER_INCREMENT, properties);
    }

    void GEAnalytics::user_max(const std::string& clientId, GEPropertiesNode& properties) {
        add(clientId, PROFILE, USER_MAX, properties);
    }

    void GEAnalytics::user_min(const std::string& clientId, GEPropertiesNode& properties) {
        add(clientId, PROFILE, USER_MIN, properties);
    }


    void GEAnalytics::user_append(const std::string& clientId, GEPropertiesNode& properties) {
        add(clientId, PROFILE, USER_APPEND, properties);
    }

    void GEAnalytics::user_uniq_append(const std::string& clientId, GEPropertiesNode& properties) {
        add(clientId, PROFILE, USER_UNIQ_APPEND, properties);
    }

    void GEAnalytics::user_unset(const std::string& clientId, GEPropertiesNode& properties) {
        add(clientId, PROFILE, USER_UNSET, properties);
    }

    void GEAnalytics::user_del(const std::string& clientId) {
        GEPropertiesNode properties;
        add(clientId, PROFILE, USER_DEL, properties);
    }

    void GEAnalytics::close() {
        m_consumer.close();
        GELog::info("SDK close");
    }

    void GEAnalytics::add(const std::string& clientId, const std::string& eventType, const std::string& eventName, GEJsonObject& properties) {

        GEJsonObject propertiesDic = properties;
        GEJsonObject finalPropertiesDic;


        finalPropertiesDic.SetString("client_id", clientId);

        std::vector<GEJsonObject> event_list;
        GEJsonObject event_list_item;


         event_list_item.SetString("type", eventType);
         event_list_item.SetString("event", eventName);
         
        int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();

         event_list_item.SetNumber("time", now);
        if (eventType == TRACK  ) {
         properties.SetString("$lib", LIB_NAME);
         properties.SetString("$lib_version", LIB_VERSION);
        }
        event_list_item.SetObject("properties", properties);
        event_list.emplace_back(event_list_item);
        finalPropertiesDic.SetList("event_list", event_list);

        std::string record = GEJsonObject::ToJson(finalPropertiesDic);

        GELog::info(record);

        m_consumer.add(finalPropertiesDic);
    }

    bool GEAnalytics::transferWithStringMap(const std::string& key, GEJsonObject& sourceProperties, GEJsonObject& destinationProperties) {
        if (sourceProperties.ContainsWithKey(key)) {
            std::shared_ptr<GEJsonObject::GEValueNode> valueNode = sourceProperties.FindNode(key);
            if (valueNode != nullptr)
            {
                destinationProperties.m_properties[key] = valueNode;
                sourceProperties.RemoveNode(key);
                return true;
            }   
        }
        return false;
    }

}
