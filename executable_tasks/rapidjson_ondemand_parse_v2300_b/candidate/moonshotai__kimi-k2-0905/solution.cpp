#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>
#include <cstdint>
#include <string>

using namespace rapidjson;

struct SumHandler {
    uint64_t sum = 0;
    enum State { START, IN_ARRAY, IN_OBJECT, GOT_ID, IN_NESTED, GOT_A, GOT_B };
    State state = START;
    std::string key;
    bool expectValue = false;

    bool Null() { if (expectValue) { expectValue = false; return true; } return false; }
    bool Bool(bool) { if (expectValue) { expectValue = false; return true; } return false; }
    bool Int(int i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Uint(unsigned u) { return Uint64(static_cast<uint64_t>(u)); }
    bool Int64(int64_t i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Uint64(uint64_t i) {
        if (!expectValue) return false;
        expectValue = false;
        switch (state) {
        case GOT_ID: sum += i; state = IN_OBJECT; break;
        case GOT_A:  sum += i; state = IN_NESTED; break;
        case GOT_B:  sum += i; state = IN_OBJECT; break;
        default: break;
        }
        return true;
    }
    bool Double(double) { if (expectValue) { expectValue = false; return true; } return false; }
    bool String(const char*, SizeType, bool) { if (expectValue) { expectValue = false; return true; } return false; }
    bool StartObject() {
        switch (state) {
        case START: return false;
        case IN_ARRAY: state = IN_OBJECT; return true;
        case IN_NESTED: return true;
        default: return false;
        }
    }
    bool Key(const char* str, SizeType len, bool) {
        if (state == IN_OBJECT) {
            if (len == 2 && str[0] == 'i' && str[1] == 'd') { state = GOT_ID; expectValue = true; return true; }
            if (len == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' && str[3] == 't' && str[4] == 'e' && str[5] == 'd') { state = IN_NESTED; return true; }
        } else if (state == IN_NESTED) {
            if (len == 1 && str[0] == 'a') { state = GOT_A; expectValue = true; return true; }
            if (len == 1 && str[0] == 'b') { state = GOT_B; expectValue = true; return true; }
        }
        return true;
    }
    bool EndObject(SizeType) {
        switch (state) {
        case IN_OBJECT: state = IN_ARRAY; return true;
        case IN_NESTED: state = IN_OBJECT; return true;
        default: return false;
        }
    }
    bool StartArray() {
        if (state == START) { state = IN_ARRAY; return true; }
        return false;
    }
    bool EndArray(SizeType) {
        if (state == IN_ARRAY) { state = START; return true; }
        return false;
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    SumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    ParseResult ok = reader.Parse(ss, handler);
    if (!ok) return 0;
    return handler.sum;
}