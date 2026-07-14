#include "interface.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/reader.h"
#include "rapidjson/internal/strfunc.h"
#include "rapidjson/error/error.h"
#include "rapidjson/rapidjson.h"

// Using RapidJSON's SAX-style parser for maximum performance on large inputs
struct ChecksumHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    int depth = 0;
    bool in_nested = false;
    bool expect_val = false;

    bool Key(const char* str, rapidjson::SizeType length, bool copy) {
        if (length == 2 && str[0] == 'i' && str[1] == 'd') expect_val = true;
        else if (length == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' && str[3] == 't' && str[4] == 'e' && str[5] == 'd') in_nested = true;
        else if (in_nested && ((length == 1 && (str[0] == 'a' || str[0] == 'b')))) expect_val = true;
        return true;
    }

    bool Uint64(uint64_t u) {
        if (expect_val) {
            sum += u;
            expect_val = false;
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