#include "interface.h"
#include <rapidjson/reader.h>
#include <string>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t total_sum = 0;
    int depth = 0;
    int current_obj_depth = 0;
    
    // State tracking for fields
    enum State { NONE, ID, NESTED, NESTED_A, NESTED_B };
    State state = NONE;
    int nested_depth = 0;

    bool Key(const char* str, SizeType length, bool copy) {
        if (depth == 2) { // Inside an object in the root array
            if (std::string_view(str, length) == "id") state = ID;
            else if (std::string_view(str, length) == "nested") state = NESTED;
            else state = NONE;
        } else if (depth == 3 && nested_depth == 1) { // Inside the 'nested' object
            if (std::string_view(str, length) == "a") state = NESTED_A;
            else if (std::string_view(str, length) == "b") state = NESTED_B;
            else state = NONE;
        }
        return true;
    }

    bool Uint64(uint64_t i) {
        if (state == ID) total_sum += i;
        else if (state == NESTED_A || state == NESTED_B) total_sum += i;
        state = NONE;
        return true;
    }

    bool Int(int i) {
        return Uint64(static_cast<uint64_t>(i));
    }

    bool Uint(unsigned i) {
        return Uint64(static_cast<uint64_t>(i));
    }

    bool StartObject() {
        depth++;
        if (depth == 3 && state == NESTED) nested_depth++;
        return true;
    }

    bool EndObject() {
        if (depth == 3 && nested_depth == 1) nested_depth--;
        depth--;
        state = NONE;
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