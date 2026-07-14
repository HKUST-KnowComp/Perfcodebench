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
        return true;
    }

    bool StartObject() {
        depth++;
        if (depth == 2) in_nested = true;
        return true;
    }

    bool EndObject() {
        if (depth == 2) in_nested = false;
        depth--;
        return true;
    }

    bool Uint64(uint64_t v) {
        if (depth == 2) {
            if (current_key == "a" || current_key == "b") {
                sum += v;
            }
        } else if (depth == 1) {
            if (current_key == "id") {
                sum += v;
            }
        }
        return true;
    }

    bool Int64(int64_t v) {
        return Uint64(static_cast<uint64_t>(v));
    }

    bool Uint(unsigned v) {
        return Uint64(static_cast<uint64_t>(v));
    }

    bool Int(int v) {
        return Uint64(static_cast<uint64_t>(v));
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    ChecksumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.sum;
}