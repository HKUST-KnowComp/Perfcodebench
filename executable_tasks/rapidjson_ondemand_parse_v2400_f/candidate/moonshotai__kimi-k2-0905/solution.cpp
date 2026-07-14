#include "interface.h"

#include <rapidjson/reader.h>
#include <rapidjson/error/en.h>
#include <cstdint>
#include <string>

struct Checker : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, Checker> {
    uint64_t sum = 0;
    enum State {
        kExpectTopArray,
        kInObject,
        kExpectIdKey,
        kReadIdValue,
        kExpectNestedKey,
        kInNested,
        kExpectAKey,
        kReadAValue,
        kExpectBKey,
        kReadBValue
    } state = kExpectTopArray;

    bool Uint64(uint64_t u) {
        switch (state) {
            case kReadIdValue:   sum += u; state = kExpectNestedKey; return true;
            case kReadAValue:    sum += u; state = kExpectBKey;     return true;
            case kReadBValue:    sum += u; state = kInObject;       return true;
            default: return true;
        }
    }
    bool Key(const char* str, rapidjson::SizeType len, bool) {
        switch (state) {
            case kExpectIdKey:
                if (len == 2 && str[0] == 'i' && str[1] == 'd') state = kReadIdValue;
                return true;
            case kExpectNestedKey:
                if (len == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' &&
                    str[3] == 't' && str[4] == 'e' && str[5] == 'd') state = kInNested;
                return true;
            case kExpectAKey:
                if (len == 1 && str[0] == 'a') state = kReadAValue;
                return true;
            case kExpectBKey:
                if (len == 1 && str[0] == 'b') state = kReadBValue;
                return true;
            default: return true;
        }
    }
    bool StartObject() {
        if (state == kExpectTopArray) state = kInObject;
        else if (state == kExpectNestedKey) state = kExpectAKey;
        return true;
    }
    bool EndObject(rapidjson::SizeType) {
        if (state == kInObject) state = kExpectTopArray;
        else if (state == kInNested || state == kExpectAKey || state == kExpectBKey) state = kInObject;
        return true;
    }
    bool StartArray() { return true; }
    bool EndArray(rapidjson::SizeType) { return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    Checker handler;
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return handler.sum;
}