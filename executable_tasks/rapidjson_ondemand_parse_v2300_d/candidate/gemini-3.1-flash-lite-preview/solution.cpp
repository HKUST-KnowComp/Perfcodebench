#include "interface.h"
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"

struct ChecksumHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    int depth = 0;
    bool in_nested = false;
    bool target_id = false;
    bool target_a = false;
    bool target_b = false;

    bool Key(const char* str, rapidjson::SizeType length, bool copy) {
        if (depth == 1) {
            target_id = (length == 2 && str[0] == 'i' && str[1] == 'd');
            in_nested = (length == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' && str[3] == 't' && str[4] == 'e' && str[5] == 'd');
        } else if (depth == 2 && in_nested) {
            target_a = (length == 1 && str[0] == 'a');
            target_b = (length == 1 && str[0] == 'b');
        }
        return true;
    }

    bool Uint64(uint64_t u) {
        if (target_id || target_a || target_b) {
            sum += u;
            target_id = target_a = target_b = false;
        }
        return true;
    }

    bool StartObject() { depth++; return true; }
    bool EndObject(rapidjson::SizeType) { depth--; in_nested = false; return true; }
    bool StartArray() { depth++; return true; }
    bool EndArray(rapidjson::SizeType) { depth--; return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    ChecksumHandler handler;
    reader.Parse(ss, handler);
    return handler.sum;
}