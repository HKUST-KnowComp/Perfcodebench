#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>
#include <cstdint>
#include <string>

using namespace rapidjson;

struct SumHandler {
    uint64_t sum = 0;
    enum class State { Start, InArray, InObject, GotId, InNested, GotA, GotB };
    State state = State::Start;
    uint64_t id = 0, a = 0, b = 0;
    int depth = 0;

    bool Null() { return true; }
    bool Bool(bool) { return true; }
    bool Int(int v) { return Uint64(static_cast<uint64_t>(v)); }
    bool Uint(unsigned v) { return Uint64(static_cast<uint64_t>(v)); }
    bool Int64(int64_t v) { return Uint64(static_cast<uint64_t>(v)); }
    bool Uint64(uint64_t v) {
        switch (state) {
        case State::GotId: id = v; state = State::InObject; return true;
        case State::GotA:  a  = v; state = State::InNested; return true;
        case State::GotB:  b  = v; state = State::InNested; sum += id + a + b; return true;
        default: return true;
        }
    }
    bool Double(double) { return true; }
    bool String(const char*, SizeType, bool) { return true; }
    bool StartObject() {
        ++depth;
        if (state == State::InArray) state = State::InObject;
        else if (state == State::InObject && depth == 2) state = State::InNested;
        return true;
    }
    bool Key(const char* str, SizeType len, bool) {
        if (state == State::InObject && len == 2 && str[0] == 'i' && str[1] == 'd') {
            state = State::GotId;
        } else if (state == State::InNested && len == 1) {
            if (str[0] == 'a') state = State::GotA;
            else if (str[0] == 'b') state = State::GotB;
        }
        return true;
    }
    bool EndObject(SizeType) {
        --depth;
        if (depth == 1) state = State::InArray;
        else if (depth == 0) state = State::Start;
        return true;
    }
    bool StartArray() {
        state = State::InArray;
        return true;
    }
    bool EndArray(SizeType) {
        state = State::Start;
        return true;
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