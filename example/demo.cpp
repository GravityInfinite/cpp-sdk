#include "../include/GEAnalytics.h"
#include "../include/GEDebugConsumer.h"
#include "../include/GEBatchConsumer.h"

#include <thread>
#include <chrono>


using namespace GEData;

// enable debug
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

const static std::string SERVER_URL = "https://backend.gravity-engine.com/event_center/api/v1/event/collect/?access_token=___XXX___";

std::unique_ptr<GEConsumer> getBatchConsumer() {
    // 批提交数据条数4，默认值20
    std::unique_ptr<GEConsumer> ptr(new GEBatchConsumer(SERVER_URL,4));
    return ptr;
}

std::unique_ptr<GEConsumer> getDebugConsumer() {
    std::unique_ptr<GEConsumer> ptr(new GEDebugConsumer(SERVER_URL));
    return ptr;
}



int main(int argc, char *argv[]) {

    GELog::enable = true;

   std::unique_ptr<GEConsumer> consumer = getDebugConsumer();

    // std::unique_ptr<GEConsumer> consumer = getBatchConsumer();

    GEAnalytics ge(*consumer);


    std::string clientId = "_test_client_id_0";
    std::string eventName = "$AdClick";

    for (int i = 0; i <= 100; ++i) {
        GEPropertiesNode properties;

        properties.SetNumber("idx", i);
        properties.SetString("name1", "2-新消息");
        properties.SetString("name2", "logBugs");
        properties.SetString("name3", "name3");
        properties.SetNumber("test_number_int", 3);
        properties.SetNumber("test_number_double", 3.14);
        properties.SetBool("test_bool", true);
        properties.SetString("test_stl_string1", "string1");

        timeb t1 = {};
        ftime(&t1);
        properties.SetDateTime("time2", t1.time, t1.millitm);

        std::vector<std::string> list;
        list.emplace_back("item11");
        list.emplace_back("item21");
        properties.SetList("test_list1", list);

        ge.track(clientId, eventName, properties);
    }



    GEPropertiesNode user_set_properties;
    user_set_properties.SetString("user_name", "test1");
    ge.user_set(clientId, user_set_properties);

    GEPropertiesNode user_set_once_properties;
    user_set_once_properties.SetString("prop_set_once", "ABC");
    ge.user_set_once(clientId, user_set_once_properties);

    GEPropertiesNode user_change_num_properties;
    user_change_num_properties.SetNumber("TotalRevenue",100);
    ge.user_increment(clientId, user_change_num_properties);

    user_change_num_properties.Clear();
    user_change_num_properties.SetNumber("TotalRevenue", 1000);
    ge.user_max(clientId, user_change_num_properties);


    user_change_num_properties.Clear();
    user_change_num_properties.SetNumber("TotalRevenue",1);
    ge.user_min(clientId, user_change_num_properties);


    GEPropertiesNode userAppend_properties;
    std::vector<std::string> userAppendListValue;
    userAppendListValue.emplace_back("11");
    userAppendListValue.emplace_back("33");
    userAppend_properties.SetList("prop_list_type", userAppendListValue);
    ge.user_append(clientId, userAppend_properties);

    GEPropertiesNode userUniqAppend_properties;
    std::vector<std::string> userUniqAppendListValue;
    userUniqAppendListValue.emplace_back("55");
    userUniqAppendListValue.emplace_back("22");
    userUniqAppendListValue.emplace_back("33");
    userUniqAppendListValue.emplace_back("66");
    userUniqAppendListValue.emplace_back("55");
    userUniqAppend_properties.SetList("prop_list_type", userUniqAppendListValue);
    ge.user_uniq_append(clientId, userUniqAppend_properties);

    GEPropertiesNode user_unset__properties;
    user_unset__properties.SetNumber("TotalRevenue", 123);
    ge.user_unset(clientId, user_unset__properties);

    ge.user_del(clientId);

    ge.flush();
    ge.close();

    return 0;
}
