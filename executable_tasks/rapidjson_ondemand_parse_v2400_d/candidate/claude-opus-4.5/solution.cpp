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
        depth++;
        if (in_array && depth == 1) {
            in_item = true;
            in_nested = false;
        }
        return true;
    }
    
    bool Key(const char* str, size_t len, bool) {
        pending_field = NONE;
        if (in_item && !in_nested && depth == 1) {
            // At item level
            if (len == 2 && str[0] == 'i' && str[1] == 'd') {
                pending_field = ID;
            } else if (len == 6 && std::memcmp(str, "nested", 6) == 0) {
                // Will enter nested object next
            }
        } else if (in_item && in_nested && depth == 2) {
            // Inside nested object
            if (len == 1) {
                if (str[0] == 'a') pending_field = NESTED_A;
                else if (str[0] == 'b') pending_field = NESTED_B;
            }
        }
        return true;
    }
    
    bool EndObject(size_t) {
        if (in_nested && depth == 2) {
            in_nested = false;
        } else if (in_item && depth == 1) {
            in_item = false;
        }
        depth--;
        pending_field = NONE;
        return true;
    }
    
    bool StartArray() {
        if (depth == 0 && !in_array) {
            in_array = true;
        }
        pending_field = NONE;
        return true;
    }
    
    bool EndArray(size_t) {
        if (depth == 0) {
            in_array = false;
        }
        pending_field = NONE;
        return true;
    }
};

// Custom handler to detect entering nested object
struct ChecksumHandlerFull : ChecksumHandler {
    bool Key(const char* str, size_t len, bool copy) {
        pending_field = NONE;
        if (in_item && !in_nested && depth == 1) {
            if (len == 2 && str[0] == 'i' && str[1] == 'd') {
                pending_field = ID;
            } else if (len == 6 && std::memcmp(str, "nested", 6) == 0) {
                in_nested = true;
            }
        } else if (in_item && in_nested && depth == 2) {
            if (len == 1) {
                if (str[0] == 'a') pending_field = NESTED_A;
                else if (str[0] == 'b') pending_field = NESTED_B;
            }
        }
        return true;
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    ChecksumHandlerFull handler;
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.sum;
}