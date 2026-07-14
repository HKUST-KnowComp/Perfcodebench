#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string json(json_input);
    
    uint64_t sum = 0;
    try {
        auto doc = parser.iterate(json);
        auto array = doc.get_array();
        
        for (auto item : array) {
            uint64_t id = 0;
            bool flag = false;
            
            // Iterate through object fields to find flag and id
            for (auto field : item.get_object()) {
                auto key = field.unescaped_key();
                if (key == "flag") {
                    flag = field.value().get_bool();
                } else if (key == "id") {
                    id = field.value().get_uint64();
                }
            }
            
            if (flag) {
                sum += id;
            }
        }
    } catch (...) {
        // In a production environment, handle parsing errors
    }
    
    return sum;
}