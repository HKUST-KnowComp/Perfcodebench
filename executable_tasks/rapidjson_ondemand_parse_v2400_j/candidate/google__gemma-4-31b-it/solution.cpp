#include "interface.h"
#include "rapidjson/reader.h"
#include <string>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    int level = 0;
    bool in_nested = false;
    const char* current_key = nullptr;

    bool Key(const char* str, SizeType length, EventHandler* abortHandler) {
        current_key = str;
        // We only care about keys at level 1 (inside array objects) or level 2 (inside 'nested')
        if (level == 1 && std::string_view(str, length) == "nested") {
            in_nested = true;
        } else if (level == 1) {
            in_nested = false;
        }
        return true;
    }

    bool Uint64(uint64_t val, EventHandler* abortHandler) {
        if (level == 1 && current_key && std::string_view(current_key) == "id") {
            sum += val;
        } else if (level == 2 && current_key) {
            if (std::string_view(current_key) == "a" || std::string_view(current_key) == "b") {
                sum += val;
            }
        }
        return true;
    }

    bool Int(int val, EventHandler* abortHandler) {
        return Uint64(static_cast<uint64_t>(val), abortHandler);
    }

    bool StartObject() { 
        level++; 
        return true; 
    }
    
    bool EndObject() { 
        level--; 
        return true; 
    }
    
    bool StartArray() { 
        level++; 
        return true; 
    }
    
    bool EndArray() { 
        level--; 
        return true; 
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    ChecksumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.sum;
}