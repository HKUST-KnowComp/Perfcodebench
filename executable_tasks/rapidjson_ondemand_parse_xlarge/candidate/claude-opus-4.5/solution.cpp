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
    enum class Expect { None, Id, NestedA, NestedB } expect = Expect::None;

    bool Null() { expect = Expect::None; return true; }
    bool Bool(bool) { expect = Expect::None; return true; }
    
    bool Int(int i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Uint(unsigned u) { return Uint64(static_cast<uint64_t>(u)); }
    bool Int64(int64_t i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Uint64(uint64_t u) {
        if (expect == Expect::Id || expect == Expect::NestedA || expect == Expect::NestedB) {
            sum += u;
        }
        expect = Expect::None;
        return true;
    }
    bool Double(double) { expect = Expect::None; return true; }
    bool RawNumber(const char*, size_t, bool) { expect = Expect::None; return true; }
    bool String(const char*, size_t, bool) { expect = Expect::None; return true; }
    
    bool Key(const char* str, size_t len, bool) {
        expect = Expect::None;
        if (depth == 2 && !in_nested) {
            if (len == 2 && str[0] == 'i' && str[1] == 'd') {
                expect = Expect::Id;
            } else if (len == 6 && std::memcmp(str, "nested", 6) == 0) {
                // Will handle in StartObject
            }
        } else if (in_nested && depth == 3) {
            if (len == 1) {
                if (str[0] == 'a') expect = Expect::NestedA;
                else if (str[0] == 'b') expect = Expect::NestedB;
            }
        }
        // Check for "nested" key at object level
        if (depth == 2 && len == 6 && std::memcmp(str, "nested", 6) == 0) {
            nested_depth = depth;
        }
        return true;
    }
    
    bool StartObject() {
        ++depth;
        if (nested_depth == depth - 1 && depth == 3) {
            in_nested = true;
        }
        expect = Expect::None;
        return true;
    }
    
    bool EndObject(size_t) {
        if (in_nested && depth == 3) {
            in_nested = false;
        }
        nested_depth = 0;
        --depth;
        expect = Expect::None;
        return true;
    }
    
    bool StartArray() {
        ++depth;
        expect = Expect::None;
        return true;
    }
    
    bool EndArray(size_t) {
        --depth;
        expect = Expect::None;
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