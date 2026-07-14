#include "interface.h"
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"

struct ChecksumHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    int depth = 0;
    bool in_nested = false;
    bool expect_id = false;
    bool expect_a = false;
    bool expect_b = false;

    bool Key(const char* str, rapidjson::SizeType length, bool copy) {
        if (depth == 1) {
            expect_id = (length == 2 && str[0] == 'i' && str[1] == 'd');
            in_nested = (length == 6 && str[0] == 'n' && str[1] == 'e');
        } else if (depth == 2 && in_nested) {
            expect_a = (length == 1 && str[0] == 'a');
            expect_b = (length == 1 && str[0] == 'b');
        }
        return true;
    }

    bool Uint64(uint64_t u) {
        if (expect_id || expect_a || expect_b) {
            sum += u;
            expect_id = expect_a = expect_b = false;
        }
        return true;
    }

    bool StartObject() { depth++; return true; }
    bool EndObject(rapidjson::SizeType memberCount) { depth--; in_nested = false; return true; }
    bool StartArray() { depth++; return true; }
    bool EndArray(rapidjson::SizeType elementCount) { depth--; return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    ChecksumHandler handler;
    reader.Parse(ss, handler);
    return handler.sum;
}