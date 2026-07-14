#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/stream.h>
#include <cstdint>
#include <string>

using namespace rapidjson;

struct Handler : public BaseReaderHandler<UTF8<>, Handler> {
    uint64_t sum = 0;
    enum class State { Start, InArray, InObject, GotId, InNested, GotA, GotB };
    State st = State::Start;
    uint64_t id = 0, a = 0, b = 0;
    bool expectKey = false;
    std::string key;

    bool Key(const char* str, SizeType len, bool) {
        key.assign(str, len);
        expectKey = false;
        return true;
    }

    bool Uint64(uint64_t u) {
        if (expectKey) return true;
        switch (st) {
          case State::InObject:
            if (key == "id") { id = u; st = State::GotId; }
            break;
          case State::InNested:
            if (key == "a") { a = u; st = State::GotA; }
            else if (key == "b") { b = u; st = State::GotB; }
            break;
          default: break;
        }
        return true;
    }

    bool StartObject() {
        if (st == State::InArray) { st = State::InObject; expectKey = true; }
        else if (st == State::GotId) { st = State::InNested; expectKey = true; }
        return true;
    }

    bool EndObject(SizeType) {
        if (st == State::GotA || st == State::GotB) {
            sum += id + a + b;
            st = State::InArray;
            id = a = b = 0;
        } else if (st == State::InObject) {
            st = State::InArray;
        }
        return true;
    }

    bool StartArray()  { if (st == State::Start) st = State::InArray; return true; }
    bool EndArray(SizeType)  { return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    Handler h;
    StringStream ss(json_input.c_str());
    Reader reader;
    reader.Parse(ss, h);
    return h.sum;
}