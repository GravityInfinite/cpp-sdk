#ifndef CPP_GE_DATA_API_H
#define CPP_GE_DATA_API_H

#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sys/timeb.h>

#include <mutex>
#include "GEConsumer.h"
#include "GEUtils.h"


namespace GEData {

    class GEAnalytics {
        
    private:
        GEConsumer& m_consumer;
        std::mutex m_mutex;

    public:
        explicit GEAnalytics(GEConsumer &consumer);
        /**
         * Construct
         * @param consumer Data consumer
         */

//        GEAnalytics(GEConsumer &consumer);
    

        /**
         * Report normal event
         * @param clientId clientId
         * @param eventName event name
         * @param properties event properties
         */
        void track(const std::string &clientId, const std::string &eventName,  GEPropertiesNode &properties);

        /**
         * Set the user attribute. If the attribute already exists, it is overwritten; otherwise, the newly created user attribute
         * @param clientId clientId

         * @param properties event properties
         */
        void user_set(const std::string &clientId,   GEPropertiesNode &properties);

        /**
         * Set the user attribute. If the attribute already exists, the operation has no effect.
         * @param clientId clientId

         * @param properties event properties
         */
        void user_set_once(const std::string &clientId,   GEPropertiesNode &properties);

        /**
         * User attribute modification, only supports the interface for adding digital attributes
         * @param clientId clientId

         * @param properties event properties
         */
        void user_increment(const std::string &clientId,   GEPropertiesNode &properties);


        /**
         * User attribute modification, only supports the interface for  set the max  digital attributes
         * @param clientId clientId

         * @param properties event properties
         */
        void user_max(const std::string &clientId,   GEPropertiesNode &properties);

                /**
         * User attribute modification, only supports the interface for set the min digital attributes
         * @param clientId clientId

         * @param properties event properties
         */
        void user_min(const std::string &clientId,   GEPropertiesNode &properties);
        /**
         * The user's array type attribute append
         * @param clientId clientId

         * @param properties event properties
         */
        void user_append(const std::string &clientId,   GEPropertiesNode &properties);

        /**
         * The attribute of the user's array type is deduplicated and appended
         * @param clientId clientId

         * @param properties event properties
         */
        void user_uniq_append(const std::string &clientId,   GEPropertiesNode &properties);

        /**
         * Delete user-specified attributes
         * @param clientId clientId

         * @param properties event attribute, the value corresponding to the key is set to 0, that is, the key attribute of the user is deleted
         */
        void user_unset(const std::string &clientId,   GEPropertiesNode &properties);

        /**
         * Delete a user, this operation is irreversible
         * @param clientId clientId

         */
        void user_del(const std::string &clientId);

        /**
         * Immediately report data to the receiving end
         */
        void flush();

        /**
         * Close and exit all threads of sdk, all local data will be cleared before stopping
         */
        void close();


    private:
        void add(const std::string &clientId, const std::string &eventType, const std::string &eventName, GEJsonObject& properties);

        static bool transferWithStringMap(const std::string &key, GEJsonObject &sourceProperties, GEJsonObject &destinationProperties);
    };


}

#endif //CPP_GE_DATA_API_H
