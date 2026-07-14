#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <string>
#include <vector>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    
    // State for the current object being parsed
    bool current_flag = false;
    bool in_target_object = false;
    bool skip_object = false;
    
    std::string current_tag;
    uint64_t current_id = 0;
    uint64_t current_value = 0;
    
    std::string current_key;

    bool StartObject() {
        in_target_object = true;
        skip_object = false;
        current_flag = false;
        current_id = 0;
        current_value = 0;
        current_tag.clear();
        return true;
    }

    bool EndObject(SizeType) {
        if (in_target_object && !skip_object && current_flag) {
            sum += current_id;
            sum += static_cast<uint64_t>(current_tag.size());
            sum += current_value;
        }
        in_target_object = false;
        return true;
    }

    bool Key(const char* str, SizeType length, bool copy) {
        current_key.assign(str, length);
        return true;
    }

    bool Bool(bool b) {
        if (in_target_object && current_key == "flag") {
            current_flag = b;
            if (!b) skip_object = true;
        }
        return true;
    }

    bool Uint64(uint64_t i) {
        if (in_target_object && !skip_object) {
            if (current_key == "id") current_id = i;
            else if (current_key == "value") current_value = i;
        }
        return true;
    }

    bool String(const char* str, SizeType length, bool copy) {
        if (in_target_object && !skip_object && current_key == "tag") {
            current_tag.assign(str, length);
        }
        return true;
    }

    // Handle cases where numbers might be parsed as Int or Double
    bool Uint(unsigned i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Int(int i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Double(double d) { 
        // The task implies uint64_t fields, but for robustness:
        uint64_t val = static_cast<uint64_t>(d);
        if (in_target_object && !skip_object) {
            if (current_key == "id") current_id = val;
            else if (current_key == "value") current_value = val;
        }
        return true; 
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    Reader reader;
    ChecksumHandler handler;
    StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.sum;
}