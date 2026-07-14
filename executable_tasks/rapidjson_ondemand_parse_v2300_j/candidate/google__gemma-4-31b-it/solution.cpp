#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    int depth = 0;
    bool in_array = false;
    bool in_object = false;
    bool in_nested = false;
    std::string current_key;

    bool Key(const char* str, SizeType length, bool copy) {
        current_key.assign(str, length);
        return true;
    }

    bool StartArray() {
        depth++;
        if (depth == 1) in_array = true;
        return true;
    }

    bool EndArray() {
        depth--;
        if (depth == 0) in_array = false;
        return true;
    }

    bool StartObject() {
        depth++;
        if (in_array && depth == 2) in_object = true;
        if (in_object && current_key == "nested") in_nested = true;
        return true;
    }

    bool EndObject() {
        if (in_nested) in_nested = false;
        if (in_object && depth == 2) in_object = false;
        depth--;
        return true;
    }

    bool Uint64(uint64_t i) {
        if (in_object) {
            if (in_nested) {
                if (current_key == "a" || current_key == "b") sum += i;
            } else {
                if (current_key == "id") sum += i;
            }
        }
        return true;
    }

    bool Int64(int64_t i) {
        return Uint64(static_cast<uint64_t>(i));
    }

    bool Uint(unsigned i) {
        return Uint64(static_cast<uint64_t>(i));
    }

    bool Int(int i) {
        return Uint64(static_cast<uint64_t>(i));
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    ChecksumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.sum;
}