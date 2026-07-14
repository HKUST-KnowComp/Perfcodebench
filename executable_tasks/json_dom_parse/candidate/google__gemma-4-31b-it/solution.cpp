#include "interface.h"
#include "simdjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string json_padded(json_input);
    simdjson::ondemand::document doc = parser.iterate(json_padded);

    uint64_t sum = 0;
    simdjson::ondemand::array_element array = doc.get_array();

    for (auto obj : array) {
        simdjson::ondemand::object item = obj.get_object();
        
        // Access fields. Using find_field is generally faster than operator[] 
        // if the order is not guaranteed, but for high performance we iterate.
        uint64_t id = 0, x = 0, val1 = 0;
        
        for (auto field : item) {
            std::string_view key = field.key();
            if (key == "id") {
                id = uint64_t(field.value());
            } else if (key == "x") {
                x = uint64_t(field.value());
            } else if (key == "vals") {
                simdjson::ondemand::array vals = field.value().get_array();
                // We need the second element (index 1)
                auto it = vals.begin();
                if (it != vals.end()) {
                    ++it;
                    if (it != vals.end()) {
                        val1 = uint64_t((*it).value());
                    }
                }
            }
        }
        sum += id + x + val1;
    }
    return sum;
}