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
    uint64_t id = 0, a = 0, b = 0;
    bool expectId = false, expectA = false, expectB = false;

    bool Null() { return true; }
    bool Bool(bool) { return true; }
    bool Int(int v) { return Uint64(static_cast<uint64_t>(v)); }
    bool Uint(unsigned v) { return Uint64(static_cast<uint64_t>(v)); }
    bool Int64(int64_t v) { return Uint64(static_cast<uint64_t>(v)); }
    bool Uint64(uint64_t v) {
        if (expectId) { id = v; expectId = false; }
        else if (expectA) { a = v; expectA = false; }
        else if (expectB) { b = v; expectB = false; }
        return true;
    }
    bool Double(double) { return true; }
    bool RawNumber(const char*, SizeType, bool) { return true; }
    bool String(const char*, SizeType, bool) { return true; }
    bool StartObject() {
        if (state == IN_ARRAY) state = IN_OBJECT;
        else if (state == IN_NESTED) { /* stay in nested */ }
        return true;
    }
    bool Key(const char* str, SizeType len, bool) {
        if (state == IN_OBJECT) {
            if (len == 2 && str[0] == 'i' && str[1] == 'd') { expectId = true; }
            else if (len == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' && str[3] == 't' && str[4] == 'e' && str[5] == 'd') { state = IN_NESTED; }
        } else if (state == IN_NESTED) {
            if (len == 1 && str[0] == 'a') { expectA = true; }
            else if (len == 1 && str[0] == 'b') { expectB = true; }
        }
        return true;
    }
    bool EndObject(SizeType) {
        if (state == IN_NESTED) { state = IN_OBJECT; }
        else if (state == IN_OBJECT) {
            sum += id + a + b;
            id = a = b = 0;
            state = IN_ARRAY;
        }
        return true;
    }
    bool StartArray() { state = IN_ARRAY; return true; }
    bool EndArray(SizeType) { return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    SumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.sum;
}