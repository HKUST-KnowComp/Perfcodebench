#include "interface.h"
#include <rapidjson/reader.h>
#include <string>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8><> {
    uint64_t total_sum = 0;
    uint64_t current_val = 0;
    int depth = 0;
    
    // State tracking for the specific fields we need
    enum State { NONE, ID, NESTED, NESTED_A, NESTED_B };
    State state = NONE;
    int nested_depth = 0;

    bool Key(const char* str, SizeType length, UnitType type) {
        if (depth == 1) {
            if (std::string_view(str, length) == "id") state = ID;
            else if (std::string_view(str, length) == "nested") state = NESTED;
            else state = NONE;
        } else if (depth == 2 && nested_depth == 1) {
            if (std::string_view(str, length) == "a") state = NESTED_A;
            else if (std::string_view(str, length) == "b") state = NESTED_B;
            else state = NONE;
        }
        return true;
    }

    bool Uint64(uint64_t v) {
        if (state == ID || state == NESTED_A || state == NESTED_B) {
            total_sum += v;
        }
        state = NONE;
        return true;
    }

    bool Int64(int64_t v) {
        if (state == ID || state == NESTED_A || state == NESTED_B) {
            total_sum += static_cast<uint64_t>(v);
        }
        state = NONE;
        return true;
    }

    bool StartObject() {
        depth++;
        if (depth == 2) nested_depth++;
        return true;
    }

    bool EndObject() {
        if (depth == 2) nested_depth--;
        depth--;
        return true;
    }

    bool StartArray() {
        depth++;
        return true;
    }

    bool EndArray() {
        depth--;
        return true;
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    ChecksumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.total_sum;
}