
#include "GEUtils.h"

#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <iomanip>
#include <utility>
#include <chrono>

namespace GEData {

#if defined(__linux__)
#define GE_SDK_LOCALTIME(seconds, now) localtime_r((seconds), (now))
#elif defined(__APPLE__)
#define GE_SDK_LOCALTIME(seconds, now) localtime_r((seconds), (now))
#elif defined(_WIN32)
#define GE_SDK_LOCALTIME(seconds, now) localtime_s((now), (seconds))
#define snprintf sprintf_s
#endif

    std::string formatDateTime(const time_t& seconds, int32_t milliseconds) {
        struct tm tm = {};
        GE_SDK_LOCALTIME(&seconds, &tm);
        char buff[64];
        snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            milliseconds);
        return buff;
    }

    static const size_t kStringPropertyValueMaxLength = 8192;

    bool CheckUtf8Valid(const std::string &str) {
        const auto *bytes = (const unsigned char *) str.data();
        const unsigned char *begin = bytes;
        while (bytes - begin < (int) str.length()) {
            if ((bytes[0] == 0x09 || bytes[0] == 0x0A || bytes[0] == 0x0D ||
                 (0x20 <= bytes[0] && bytes[0] <= 0x7E))) {
                bytes += 1;
                continue;
            }
            if (((0xC2 <= bytes[0] && bytes[0] <= 0xDF)
                 && (0x80 <= bytes[1] && bytes[1] <= 0xBF))) {
                bytes += 2;
                continue;
            }
            if ((bytes[0] == 0xE0 && (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF)) ||
                (((0xE1 <= bytes[0] && bytes[0] <= 0xEC) || bytes[0] == 0xEE
                  || bytes[0] == 0xEF) &&
                 (0x80 <= bytes[1] && bytes[1] <= 0xBF)
                 && (0x80 <= bytes[2] && bytes[2] <= 0xBF)) ||
                (bytes[0] == 0xED && (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF))) {
                bytes += 3;
                continue;
            }
            if ((bytes[0] == 0xF0 && (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                 (0x80 <= bytes[3] && bytes[3] <= 0xBF)) ||
                ((0xF1 <= bytes[0] && bytes[0] <= 0xF3)
                 && (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF)
                 && (0x80 <= bytes[3] && bytes[3] <= 0xBF)) ||
                (bytes[0] == 0xF4 && (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                 (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                 (0x80 <= bytes[3] && bytes[3] <= 0xBF))) {
                bytes += 4;
                continue;
            }
            return false;
        }
        return bytes - begin == str.length();
    }

    /*********************************  GEJsonObject  *********************************/

    GEJsonObject::GEJsonObject() = default;

    void GEJsonObject::SetNumber(const std::string& property_name, double value) {
        m_properties[property_name] = std::make_shared<GEValueNode>(value);
    }

    void GEJsonObject::SetNumber(const std::string& property_name, int32_t value) {
        m_properties[property_name] = std::make_shared<GEValueNode>(static_cast<int64_t>(value));
    }

    void GEJsonObject::SetNumber(const std::string& property_name, int64_t value) {

        m_properties[property_name] = std::make_shared<GEValueNode>(value);
    }

    void GEJsonObject::SetString(const std::string &property_name, const std::string &value) {
        if (value.length() > kStringPropertyValueMaxLength) {
            std::cerr << "String property '" << property_name
                      << "' is too int64_t, value: " << value << std::endl;
            return;
        }
        if (!CheckUtf8Valid(value)) {
            std::cerr << "String property '" << property_name
                      << "' is not valid UTF-8 std::string, value: " << value
                      << std::endl;
            return;
        }


        m_properties[property_name] = std::make_shared<GEValueNode>(value);
    }

    void GEJsonObject::SetString(const std::string &property_name, const char *value) {
        SetString(property_name, std::string(value));
    }

    void GEJsonObject::SetBool(const std::string &property_name, bool value) {

        m_properties[property_name] = std::make_shared<GEValueNode>(value);
    }

    void GEJsonObject::SetObject(const std::string &property_name, const GEJsonObject &value) {

        m_properties[property_name] = std::make_shared<GEValueNode>(value);
    }

    void GEJsonObject::SetList(const std::string &property_name,
                               const std::vector<std::string> &value) {

        m_properties[property_name] = std::make_shared<GEValueNode>(value);
    }

    void GEJsonObject::SetList(const std::string &property_name,
                               const std::vector<GEJsonObject> &value) {

        m_properties[property_name] = std::make_shared<GEValueNode>(value);
    }

    void GEJsonObject::SetDateTime(const std::string &property_name,
                                   const time_t seconds,
                                   int32_t milliseconds) {

        m_properties[property_name] = std::make_shared<GEValueNode>(seconds, milliseconds);
    }

    void GEJsonObject::SetDateTime(const std::string &property_name,
                                   const std::string &value) {

        m_properties[property_name] = std::make_shared<GEValueNode>(value);
    }

    void GEJsonObject::Clear() {
        m_properties.clear();
    }

    void GEJsonObject::DumpNode(const GEJsonObject& node, rapidjson::Writer<rapidjson::StringBuffer>& writer) {
        writer.StartObject();

        for (const auto & pairValue : node.m_properties) {

            writer.Key(pairValue.first.c_str());

            GEValueNode& contentNode = *(pairValue.second);

            switch (contentNode.node_type_) {
            case NUMBER: {
                writer.Double(contentNode.value_.number_value);
            } break;
            case INT: {
                writer.Int64(contentNode.value_.int_value);
            } break;
            case STRING: {
                writer.String(contentNode.m_data_string.c_str());
            } break;
            case LIST: {
                writer.StartArray();

                for (const std::string& var : contentNode.list_data_)
                {
                    writer.String(var.c_str());
                }

                writer.EndArray();
            } break;
            case GE_BOOL: {
                writer.Bool(contentNode.value_.bool_value);
            } break;
            case OBJECT: {
                DumpNode(contentNode.object_data_, writer);
            } break;
            case DATETIME: {
                std::string str = formatDateTime(contentNode.value_.date_time_value.seconds, contentNode.value_.date_time_value.milliseconds);
                writer.String(str.c_str());
            } break;
            case OBJECTS: {
                writer.StartArray();

                for (const GEJsonObject& var : contentNode.list_obj_)
                {
                    DumpNode(var, writer);
                }

                writer.EndArray();
            } break;
            default:
                break;
            }
        }

        writer.EndObject();
    }

    std::string GEJsonObject::ToJson(const GEJsonObject& node) {

        rapidjson::StringBuffer strBuf;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuf);

        DumpNode(node, writer);

        std::string buffer = strBuf.GetString();

        return buffer;
    }

    void GEJsonObject::MergeFrom(const GEJsonObject& another_node) {
        for (const auto & pairValue : another_node.m_properties) {
            m_properties[pairValue.first] = pairValue.second;
        }
    }

    bool GEJsonObject::ContainsWithKey(const std::string& key) {
        std::map<std::string, std::shared_ptr<GEValueNode>>::const_iterator iter = m_properties.find(key);

        if (iter != m_properties.end())
        {
            return true;
        }
        else {
            return false;
        }
    }
   
    std::shared_ptr<GEJsonObject::GEValueNode> GEJsonObject::FindNode(const std::string& key) {
        std::map<std::string, std::shared_ptr<GEValueNode>>::const_iterator iter = m_properties.find(key);
        if (iter != m_properties.end() && iter->first == key) {
            return iter->second;
        }
        return nullptr;
    }

    bool GEJsonObject::RemoveNode(const std::string& key) {
        return m_properties.erase(key);
    }

    /*********************************  GEValueNode  *********************************/
    
    GEJsonObject::GEValueNode::GEValueNode(double value) : node_type_(NUMBER) {
        value_.number_value = value;
    }

    GEJsonObject::GEValueNode::GEValueNode(int64_t value) : node_type_(INT) {
        value_.int_value = value;
    }

    GEJsonObject::GEValueNode::GEValueNode(std::string value)
            : node_type_(STRING),
              m_data_string(std::move(value)) {}

    GEJsonObject::GEValueNode::GEValueNode(bool value) : node_type_(GE_BOOL) {
        value_.bool_value = value;
    }

    GEJsonObject::GEValueNode::GEValueNode(const GEJsonObject &value)
            : node_type_(OBJECT) {
        object_data_ = value;
    }

    GEJsonObject::GEValueNode::GEValueNode(const std::vector<std::string> &value)
            : node_type_(LIST),
              list_data_(value) {}

    GEJsonObject::GEValueNode::GEValueNode(const std::vector<GEJsonObject> &value)
            : node_type_(OBJECTS),
              list_obj_(value) {}

    GEJsonObject::GEValueNode::GEValueNode(time_t seconds, int32_t milliseconds)
            : node_type_(DATETIME) {
        value_.date_time_value.seconds = seconds;
        value_.date_time_value.milliseconds = milliseconds;
    }

    bool GELog::enable = false;
    void GELog::info(const std::string & message) {
        if (GELog::enable) {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm* timeInfo = std::localtime(&now_c);
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
            std::cout << "[GEData]" << "[" << buffer << "] " <<  message << std::endl;
        }
    }
}