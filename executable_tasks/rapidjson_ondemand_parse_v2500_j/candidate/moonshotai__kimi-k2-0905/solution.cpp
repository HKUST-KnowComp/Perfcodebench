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
        case State::InObject:
            if (lastKey == "id") { id = v; state = State::GotId; }
            break;
        case State::InNested:
            if (lastKey == "a") { a = v; state = State::GotA; }
            else if (lastKey == "b") { b = v; state = State::GotB; }
            break;
        default: break;
        }
        return true;
    }
    bool Double(double) { return true; }
    bool String(const char*, SizeType, bool) { return true; }
    bool StartObject() {
        ++depth;
        switch (state) {
        case State::InArray: state = State::InObject; break;
        case State::GotId:   state = State::InNested; break;
        default: break;
        }
        return true;
    }
    bool EndObject(SizeType) {
        --depth;
        if (state == State::GotB && depth == 1) {
            sum += id + a + b;
            state = State::InArray;
        } else if (state == State::InNested) {
            state = State::GotId;
        }
        return true;
    }
    bool StartArray() {
        if (state == State::Start) state = State::InArray;
        return true;
    }
    bool EndArray(SizeType) { return true; }

    std::string lastKey;
    bool Key(const char* str, SizeType len, bool) {
        lastKey.assign(str, len);
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