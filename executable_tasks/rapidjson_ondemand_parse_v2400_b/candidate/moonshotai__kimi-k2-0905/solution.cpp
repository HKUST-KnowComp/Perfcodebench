#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>
#include <cstdint>
#include <string>

namespace {

struct Handler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, Handler> {
    uint64_t sum = 0;
    enum class State {
        Start, InArray, InObject, GotId, InNested, GotA, GotB
    } state = State::Start;

    bool Uint64(uint64_t u) {
        switch (state) {
        case State::GotId:
            sum += u;
            state = State::InObject;
            return true;
        case State::GotA:
            sum += u;
            state = State::InNested;
            return true;
        case State::GotB:
            sum += u;
            state = State::InObject;
            return true;
        default:
            return true;
        }
    }

    bool Key(const char* str, rapidjson::SizeType len, bool) {
        switch (state) {
        case State::InObject:
            if (len == 2 && str[0] == 'i' && str[1] == 'd') {
                state = State::GotId;
            } else if (len == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' && str[3] == 't' && str[4] == 'e' && str[5] == 'd') {
                state = State::InNested;
            }
            return true;
        case State::InNested:
            if (len == 1 && str[0] == 'a') {
                state = State::GotA;
            } else if (len == 1 && str[0] == 'b') {
                state = State::GotB;
            }
            return true;
        default:
            return true;
        }
    }

    bool StartArray()       { if (state == State::Start) state = State::InArray; return true; }
    bool EndArray()         { return true; }
    bool StartObject()      {
        if (state == State::InArray) state = State::InObject;
        else if (state == State::InNested) state = State::InNested;
        return true;
    }
    bool EndObject()        {
        if (state == State::InObject) state = State::InArray;
        else if (state == State::InNested) state = State::InObject;
        return true;
    }
};

} // anonymous

uint64_t compute_checksum(const std::string& json_input) {
    Handler h;
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    rapidjson::ParseResult ok = reader.Parse(ss, h);
    (void)ok; // we assume input is valid JSON per task statement
    return h.sum;
}