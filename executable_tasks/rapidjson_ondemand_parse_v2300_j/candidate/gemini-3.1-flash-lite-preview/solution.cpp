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
        if (length == 2 && str[0] == 'i' && str[1] == 'd') target_id = true;
        else if (length == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' && str[3] == 't' && str[4] == 'e' && str[5] == 'd') in_nested = true;
        else if (in_nested && length == 1 && str[0] == 'a') target_a = true;
        else if (in_nested && length == 1 && str[0] == 'b') target_b = true;
        return true;
    }

    bool Uint64(uint64_t u) {
        if (target_id || target_a || target_b) {
            sum += u;
            target_id = target_a = target_b = false;
        }
        return true;
    }

    bool EndObject(rapidjson::SizeType memberCount) {
        in_nested = false;
        return true;
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    ChecksumHandler handler;
    reader.Parse(ss, handler);
    return handler.sum;
}