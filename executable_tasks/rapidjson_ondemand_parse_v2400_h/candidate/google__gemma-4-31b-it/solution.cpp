#include "interface.h"
#include <rapidjson/reader.h>
#include <string>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    int level = 0;
    int current_obj_level = 0;
    std::string current_key;
    bool in_nested = false;

    bool Key(const char* str, SizeType length, bool copy) {
        current_key.assign(str, length);
        if (level == 2 && current_key == "nested") {
            in_nested = true;
        } else if (level == 3) {
            in_nested = false;
        }
        return true;
    }

    bool Uint64(uint64_t i) {
        if (level == 2 && current_key == "id") {
            sum += i;
        } else if (level == 3 && (current_key == "a" || current_key == "b")) {
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
        level++;
        return true;
    }

    bool EndObject() {
        level--;
        if (level == 1) in_nested = false;
        return true;
    }

    bool StartArray() {
        level++;
        return true;
    }

    bool EndArray() {
        level--;
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