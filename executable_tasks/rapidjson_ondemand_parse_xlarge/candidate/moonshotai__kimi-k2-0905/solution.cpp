#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/stream.h>
#include <cstdint>
#include <string>

using namespace rapidjson;

struct Handler : public BaseReaderHandler<UTF8<>, Handler> {
    uint64_t sum = 0;
    enum State { START, IN_ARRAY, IN_OBJECT, IN_NESTED, KEY_ID, KEY_NESTED, KEY_A, KEY_B, VALUE } state = START;
    uint64_t current_id = 0, current_a = 0, current_b = 0;
    bool in_nested = false;

    bool Key(const char* str, SizeType len, bool) {
        if (state == IN_OBJECT) {
            if (len == 2 && str[0] == 'i' && str[1] == 'd') { state = KEY_ID; }
            else if (len == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' && str[3] == 't' && str[4] == 'e' && str[5] == 'd') { state = KEY_NESTED; }
        } else if (state == IN_NESTED) {
            if (len == 1 && str[0] == 'a') { state = KEY_A; }
            else if (len == 1 && str[0] == 'b') { state = KEY_B; }
        }
        return true;
    }

    bool Uint64(uint64_t u) {
        switch (state) {
            case KEY_ID: current_id = u; state = IN_OBJECT; break;
            case KEY_A: current_a = u; state = IN_NESTED; break;
            case KEY_B: current_b = u; state = IN_NESTED; break;
            default: break;
        }
        return true;
    }

    bool StartObject() {
        if (state == IN_ARRAY) state = IN_OBJECT;
        else if (state == KEY_NESTED) { state = IN_NESTED; in_nested = true; }
        return true;
    }

    bool EndObject(SizeType) {
        if (in_nested) { state = IN_OBJECT; in_nested = false; }
        else if (state == IN_OBJECT) {
            sum += current_id + current_a + current_b;
            current_id = current_a = current_b = 0;
            state = IN_ARRAY;
        }
        return true;
    }

    bool StartArray() { state = IN_ARRAY; return true; }
    bool EndArray(SizeType) { state = START; return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    Handler h;
    StringStream ss(json_input.c_str());
    Reader reader;
    reader.Parse(ss, h);
    return h.sum;
}