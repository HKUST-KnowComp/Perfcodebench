#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>
#include <string>
#include <vector>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t total_sum = 0;
    
    // State tracking
    enum State { ROOT, OBJECT, NESTED_OBJECT };
    State current_state = ROOT;
    
    std::string current_key;
    bool in_nested = false;
    bool in_target_object = false;

    bool Key(const char* str, SizeType length, bool copy) {
        current_key.assign(str, length);
        return true;
    }

    bool StartObject() {
        if (current_state == ROOT) {
            in_target_object = true;
        } else if (current_key == "nested") {
            in_nested = true;
        }
        return true;
    }

    bool EndObject(SizeType memberCount) {
        if (in_nested) {
            in_nested = false;
        } else if (in_target_object) {
            in_target_object = false;
        }
        return true;
    }

    bool Uint64(uint64_t i) {
        if (in_target_object) {
            if (current_key == "id") {
                total_sum += i;
            } else if (in_nested) {
                if (current_key == "a" || current_key == "b") {
                    total_sum += i;
                }
            }
        }
        return true;
    }

    // Handle cases where numbers might be parsed as Int64 or Double
    bool Int64(int64_t i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Uint(unsigned i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Double(double d) { 
        total_sum += static_cast<uint64_t>(d);
        return true; 
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    Reader reader;
    StringStream ss(json_input.c_str(), json_input.size());
    ChecksumHandler handler;

    reader.Parse(ss, handler);

    return handler.total_sum;
}