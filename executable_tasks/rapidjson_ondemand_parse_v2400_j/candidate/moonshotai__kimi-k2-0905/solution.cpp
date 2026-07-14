#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <sstream>
#include <cstdint>

using namespace rapidjson;

struct SumHandler {
    uint64_t sum = 0;
    enum State { START, IN_ARRAY, IN_OBJECT, GOT_ID, IN_NESTED, GOT_A, GOT_B };
    State state = START;
    std::string key;
    bool expectValue = false;

    bool Null() { return true; }
    bool Bool(bool) { return true; }
    bool Int(int v) { return Number(static_cast<uint64_t>(v)); }
    bool Uint(unsigned v) { return Number(static_cast<uint64_t>(v)); }
    bool Int64(int64_t v) { return Number(static_cast<uint64_t>(v)); }
    bool Uint64(uint64_t v) { return Number(v); }
    bool Double(double) { return true; }
    bool RawNumber(const char*, SizeType, bool) { return true; }
    bool String(const char*, SizeType, bool) { return true; }

    bool Number(uint64_t v) {
        if (expectValue) {
            if (state == GOT_ID) { sum += v; state = IN_OBJECT; expectValue = false; }
            else if (state == GOT_A) { sum += v; state = IN_NESTED; expectValue = false; }
            else if (state == GOT_B) { sum += v; state = IN_NESTED; expectValue = false; }
        }
        return true;
    }

    bool Key(const char* str, SizeType len, bool) {
        key.assign(str, len);
        expectValue = true;
        if (state == IN_OBJECT) {
            if (key == "id") state = GOT_ID;
            else if (key == "nested") state = IN_NESTED;
            else expectValue = false;
        } else if (state == IN_NESTED) {
            if (key == "a") state = GOT_A;
            else if (key == "b") state = GOT_B;
            else expectValue = false;
        }
        return true;
    }

    bool StartObject() {
        if (state == IN_ARRAY) state = IN_OBJECT;
        else if (state == IN_NESTED) { /* stay in nested */ }
        return true;
    }
    bool EndObject(SizeType) {
        if (state == IN_OBJECT || state == GOT_ID) state = IN_ARRAY;
        else if (state == IN_NESTED || state == GOT_A || state == GOT_B) state = IN_OBJECT;
        expectValue = false;
        return true;
    }
    bool StartArray() { state = IN_ARRAY; return true; }
    bool EndArray(SizeType) { state = START; return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    SumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    if (!reader.Parse(ss, handler)) {
        // On parse error return 0 (undefined per task, but we must return something)
        return 0;
    }
    return handler.sum;
}