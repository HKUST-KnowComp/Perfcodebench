#include "interface.h"

#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>
#include <cstdint>
#include <cstring>

struct ChecksumHandler {
    uint64_t sum = 0;
    
    // State tracking
    int depth = 0;           // Current object nesting depth
    bool in_array = false;   // Are we inside the top-level array?
    bool in_item = false;    // Are we inside an array item object?
    bool in_nested = false;  // Are we inside the "nested" object?
    
    // Which field are we expecting a value for?
    enum Field { NONE, ID, NESTED_A, NESTED_B };
    Field pending_field = NONE;
    
    bool Null() { pending_field = NONE; return true; }
    bool Bool(bool) { pending_field = NONE; return true; }
    
    bool Int(int i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Uint(unsigned u) { return Uint64(static_cast<uint64_t>(u)); }
    bool Int64(int64_t i) { return Uint64(static_cast<uint64_t>(i)); }
    
    bool Uint64(uint64_t u) {
        if (pending_field != NONE) {
            sum += u;
            pending_field = NONE;
        }
        return true;
    }
    
    bool Double(double) { pending_field = NONE; return true; }
    bool RawNumber(const char*, size_t, bool) { pending_field = NONE; return true; }
    bool String(const char*, size_t, bool) { pending_field = NONE; return true; }
    
    bool StartObject() {
        if (in_array && !in_item) {
            in_item = true;
            depth = 1;
        } else if (in_item) {
            depth++;
        }
        return true;
    }
    
    bool Key(const char* str, size_t len, bool) {
        pending_field = NONE;
        if (in_item && depth == 1) {
            // Top-level keys in item
            if (len == 2 && str[0] == 'i' && str[1] == 'd') {
                pending_field = ID;
            } else if (len == 6 && std::memcmp(str, "nested", 6) == 0) {
                in_nested = true;
            }
        } else if (in_item && depth == 2 && in_nested) {
            // Keys inside "nested" object
            if (len == 1) {
                if (str[0] == 'a') pending_field = NESTED_A;
                else if (str[0] == 'b') pending_field = NESTED_B;
            }
        }
        return true;
    }
    
    bool EndObject(size_t) {
        if (in_item) {
            if (depth == 2 && in_nested) {
                in_nested = false;
            }
            depth--;
            if (depth == 0) {
                in_item = false;
                in_nested = false;
            }
        }
        pending_field = NONE;
        return true;
    }
    
    bool StartArray() {
        if (!in_array) {
            in_array = true;
        }
        pending_field = NONE;
        return true;
    }
    
    bool EndArray(size_t) {
        pending_field = NONE;
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