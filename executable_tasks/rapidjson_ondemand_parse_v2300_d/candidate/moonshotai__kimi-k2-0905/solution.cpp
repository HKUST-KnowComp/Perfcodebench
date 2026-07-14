#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>
#include <cstdint>
#include <string>

namespace {

struct Handler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, Handler> {
    uint64_t sum = 0;
    enum class State {
        START,
        IN_ARRAY,
        IN_OBJECT,
        GOT_ID,
        IN_NESTED,
        GOT_A,
        GOT_B
    } state = State::START;

    uint64_t id = 0;
    uint64_t a  = 0;
    uint64_t b  = 0;

    bool Uint64(uint64_t u) {
        switch (state) {
        case State::IN_OBJECT:
            id = u;
            state = State::GOT_ID;
            return true;
        case State::IN_NESTED:
            if (!got_a) { a = u; got_a = true; state = State::GOT_A; }
            else        { b = u; got_b = true; state = State::GOT_B; }
            return true;
        default:
            return true;
        }
    }

    bool StartArray() {
        if (state == State::START) { state = State::IN_ARRAY; }
        return true;
    }
    bool EndArray(SizeType) {
        return true;
    }

    bool StartObject() {
        if (state == State::IN_ARRAY) {
            state = State::IN_OBJECT;
            got_id = got_a = got_b = false;
        } else if (state == State::GOT_ID) {
            state = State::IN_NESTED;
            got_a = got_b = false;
        }
        return true;
    }

    bool EndObject(SizeType) {
        switch (state) {
        case State::IN_OBJECT:
            state = State::IN_ARRAY;
            return true;
        case State::GOT_A:
        case State::GOT_B:
            sum += id + a + b;
            state = State::IN_ARRAY;
            return true;
        default:
            return true;
        }
    }

    bool Key(const char* str, SizeType len, bool) {
        if (state == State::IN_OBJECT && len == 2 && str[0]=='i' && str[1]=='d') {
            return true;
        }
        if (state == State::IN_NESTED) {
            if (len == 1 && str[0]=='a') return true;
            if (len == 1 && str[0]=='b') return true;
        }
        return true;
    }

private:
    bool got_id = false, got_a = false, got_b = false;
};

} // anonymous

uint64_t compute_checksum(const std::string& json_input) {
    Handler h;
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    rapidjson::ParseResult ok = reader.Parse(ss, h);
    if (!ok) {
        // On malformed input we return 0 to match baseline behavior
        return 0;
    }
    return h.sum;
}