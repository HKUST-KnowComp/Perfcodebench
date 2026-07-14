#include "interface.h"
#include <rapidjson/reader.h>
#include <string>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    int depth = 0;
    bool in_nested = false;
    const char* current_key = nullptr;

    bool Key(const char* str, SizeType length, bool copy) {
        current_key = str;
        // We only care about keys at depth 1 (id, nested) or depth 2 (a, b)
        return true;
    }

    bool StartObject() {
        depth++;
        if (depth == 2 && current_key && std::string_view(current_key).find("nested") != std::string::npos) {
            in_nested = true;
        }
        return true;
    }

    bool EndObject() {
        if (depth == 2) in_nested = false;
        depth--;
        return true;
    }

    bool Uint64(uint64_t i) {
        if (!current_key) return true;
        
        if (depth == 1) {
            if (current_key[0] == 'i' && std::string_view(current_key) == "id") {
                sum += i;
            }
        } else if (depth == 2 && in_nested) {
            if (current_key[0] == 'a' && std::string_view(current_key) == "a") {
                sum += i;
            } else if (current_key[0] == 'b' && std::string_view(current_key) == "b") {
                sum += i;
            }
        }
        return true;
    }

    bool Int(int i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Int64(int64_t i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Uint(unsigned i) { return Uint64(static_cast<uint64_t>(i)); }
};

uint64_t compute_checksum(const std::string& json_input) {
    ChecksumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.sum;
}