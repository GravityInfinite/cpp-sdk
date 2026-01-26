
#ifndef CPP_GE_UTILS_H
#define CPP_GE_UTILS_H

#include <iostream>
#include <map>
#include <vector>
#include <regex>
#include <sstream>
#include <cstring>
#include "GEJsonParse.h"


#define ErrorLog(errorMsg) std::cerr << "ERROR: [" << __FILE__ << " -> line number: " << __LINE__ << " -> function name: " << __FUNCTION__ << "][error message: " << errorMsg << "" << "]" << std::endl;

const std::string kPathSeparator =
#if defined _WIN32 || defined __CYGWIN__
        "\\";
#else
"/";
#endif


namespace GEData {



    std::string formatDateTime(const time_t& seconds, int32_t milliseconds);

    /*!
    * GEJsonObject class
     * Mainly used to process the map attribute m_properties in the GEJsonObject class
     */
    class GEJsonObject {
    public:
        void SetNumber(const std::string &property_name, int32_t value);

        void SetNumber(const std::string &property_name, int64_t value);

        void SetNumber(const std::string &property_name, double value);

        void SetString(const std::string &property_name, const std::string &value);

        void SetString(const std::string &property_name, const char *value);

        void SetBool(const std::string &property_name, bool value);

        void SetList(const std::string &property_name, const std::vector<std::string> &value);

        void SetObject(const std::string &property_name, const GEJsonObject &value);

        void SetList(const std::string &property_name, const std::vector<GEJsonObject> &value);

        void SetDateTime(const std::string &property_name, time_t seconds,
                         int32_t milliseconds);

        void SetDateTime(const std::string &property_name, const std::string &value);

        void Clear();

        static std::string ToJson(const GEJsonObject &node);

        GEJsonObject();

        class GEValueNode;

        std::map<std::string, std::shared_ptr<GEValueNode>> m_properties;

        static void DumpNode(const GEJsonObject& node, rapidjson::Writer<rapidjson::StringBuffer> &writer);

        void MergeFrom(const GEJsonObject &another_node);

        bool ContainsWithKey(const std::string &key);

        std::shared_ptr<GEValueNode> FindNode(const std::string& key);

        bool RemoveNode(const std::string &key);

        enum ValueNodeType {
            NUMBER,
            INT,
            STRING,
            LIST,
            DATETIME,
            GE_BOOL,
            OBJECT,
            OBJECTS,
            UNKNOWN,
        };
    };

    class GEJsonObject::GEValueNode {
    public:

        ValueNodeType node_type_;

        GEValueNode() : node_type_(UNKNOWN) {}

        explicit GEValueNode(double value);

        explicit GEValueNode(int64_t value);

        explicit GEValueNode(std::string value);

        explicit GEValueNode(bool value);

        explicit GEValueNode(const GEJsonObject &value);

        explicit GEValueNode(const std::vector<std::string> &value);

        explicit GEValueNode(const std::vector<GEJsonObject> &value);

        GEValueNode(time_t seconds, int32_t milliseconds);

        GEJsonObject object_data_;
        std::string m_data_string;
        std::vector<std::string> list_data_;
        std::vector<GEJsonObject> list_obj_;

        union UnionValue {
            double number_value{};
            bool bool_value;
            struct {
                std::time_t seconds;
                int32_t milliseconds;
            } date_time_value;
            int64_t int_value;

            UnionValue() { memset(this, 0, sizeof(UnionValue)); }
        } value_;
    };

    /**
     * Event properties node
     */
    class GEPropertiesNode : public GEJsonObject {};

    class GELog {
    public:
       static bool enable;
       static void info(const std::string& message);
    };
}

#endif //CPP_GE_UTILS_H
