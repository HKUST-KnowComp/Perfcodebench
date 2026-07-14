#include "interface.h"

#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>
#include <cstdint>
#include <cstring>

struct ChecksumHandler {
    uint64_t sum = 0;
    int depth = 0;
    bool in_nested = false;
    int nested_depth = 0;
    
    // Track which field we're expecting a value for
    // 0 = none, 1 = id, 2 = nested.a, 3 = nested.b
    int expecting = 0;
    
    bool Null() { expecting = 0; return true; }
    bool Bool(bool) { expecting = 0; return true; }
    
    bool Int(int i) {
        if (expecting == 1 || expecting == 2 || expecting == 3) {
            sum += static_cast<uint64_t>(i);
        }
        expecting = 0;
        return true;
    }
    
    bool Uint(unsigned u) {
        if (expecting == 1 || expecting == 2 || expecting == 3) {
            sum += static_cast<uint64_t>(u);
        }
        expecting = 0;
        return true;
    }
    
    bool Int64(int64_t i) {
        if (expecting == 1 || expecting == 2 || expecting == 3) {
            sum += static_cast<uint64_t>(i);
        }
        expecting = 0;
        return true;
    }
    
    bool Uint64(uint64_t u) {
        if (expecting == 1 || expecting == 2 || expecting == 3) {
            sum += u;
        }
        expecting = 0;
        return true;
    }
    
    bool Double(double) { expecting = 0; return true; }
    bool RawNumber(const char*, size_t, bool) { expecting = 0; return true; }
    bool String(const char*, size_t, bool) { expecting = 0; return true; }
    
    bool StartObject() {
        depth++;
        if (in_nested) {
            nested_depth++;
        }
        expecting = 0;
        return true;
    }
    
    bool Key(const char* str, size_t len, bool) {
        expecting = 0;
        
        // At depth 2, we're inside an array item object
        if (depth == 2 && !in_nested) {
            if (len == 2 && str[0] == 'i' && str[1] == 'd') {
                expecting = 1; // id
            } else if (len == 6 && std::memcmp(str, "nested", 6) == 0) {
                in_nested = true;
                nested_depth = 0;
            }
        }
        // Inside nested object at the first level
        else if (in_nested && nested_depth == 1) {
            if (len == 1) {
                if (str[0] == 'a') {
                    expecting = 2; // nested.a
                } else if (str[0] == 'b') {
                    expecting = 3; // nested.b
                }
            }
        }
        
        return true;
    }
    
    bool EndObject(size_t) {
        if (in_nested) {
            nested_depth--;
            if (nested_depth == 0) {
                in_nested = false;
            }
        }
        depth--;
        expecting = 0;
        return true;
    }
    
    bool StartArray() {
        expecting = 0;
        return true;
    }
    
    bool EndArray(size_t) {
        expecting = 0;
        return true;
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    ChecksumHandler handler;
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.sum;
}