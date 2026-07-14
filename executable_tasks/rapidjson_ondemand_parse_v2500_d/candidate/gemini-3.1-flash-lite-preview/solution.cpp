#include "interface.h"
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"

struct ChecksumHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    int depth = 0;
    bool in_nested = false;
    bool target_field = false;

    bool Uint64(uint64_t u) {
        if (target_field) {
            sum += u;
            target_field = false;
        }
        return true;
    }

    bool Key(const char* str, rapidjson::SizeType length, bool copy) {
        if (depth == 1) {
            target_field = (length == 2 && str[0] == 'i' && str[1] == 'd');
        } else if (in_nested) {
            target_field = (length == 1 && (str[0] == 'a' || str[0] == 'b'));
        }
        return true;
    }

    bool StartObject() {
        depth++;
        return true;
    }

    bool EndObject(rapidjson::SizeType memberCount) {
        depth--;
        in_nested = false;
        return true;
    }

    bool StartObject(const char* name, rapidjson::SizeType length) {
        if (length == 6 && strncmp(name, "nested", 6) == 0) {
            in_nested = true;
        }
        return true;
    }

    bool StartObject() { return true; }
    bool EndObject(rapidjson::SizeType) { depth--; in_nested = false; return true; }
};

// Simplified handler for the specific schema requirements
struct FastHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, FastHandler> {
    uint64_t sum = 0;
    int depth = 0;
    bool in_nested = false;
    bool expect_val = false;

    bool Key(const char* str, rapidjson::SizeType len, bool) {
        if (depth == 1 && len == 2 && str[0] == 'i' && str[1] == 'd') expect_val = true;
        else if (depth == 2 && in_nested && len == 1 && (str[0] == 'a' || str[0] == 'b')) expect_val = true;
        else if (depth == 1 && len == 6 && str[0] == 'n') in_nested = true;
        return true;
    }
    bool Uint64(uint64_t u) { if (expect_val) { sum += u; expect_val = false; } return true; }
    bool StartObject() { depth++; return true; }
    bool EndObject(rapidjson::SizeType) { depth--; in_nested = false; return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    FastHandler handler;
    reader.Parse(ss, handler);
    return handler.sum;
}