#include "interface.h"
#include <rapidjson/reader.h>
#include <string>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    int depth = 0;
    bool in_nested = false;
    std::string current_key;

    bool Key(const char* str, SizeType length, bool copy) {
        current_key.assign(str, length);
        if (depth == 2 && current_key == "nested") {
            in_nested = true;
        } else if (depth == 3) {
            in_nested = false; // Reset for siblings of 'nested' if any
        } else if (depth == 2) {
            in_nested = false;
        }
        return true;
    }

    bool Uint64(uint64_t i) {
        if (depth == 2 && current_key == "id") {
            sum += i;
        } else if (depth == 3 && (current_key == "a" || current_key == "b")) {
            sum += i;
        }
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
        return true;
    }

    bool EndObject() {
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
    return handler.sum;
}