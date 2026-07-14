#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>

namespace {

struct ChecksumHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    enum class Expecting {
        NONE,
        VALUE
    } state = Expecting::NONE;

    // Handle all numeric types that might represent the uint64_t values
    bool Uint(unsigned u) {
        if (state == Expecting::VALUE) {
            sum += u;
            state = Expecting::NONE;
        }
        return true;
    }

    bool Int(int i) {
        if (state == Expecting::VALUE) {
            sum += static_cast<uint64_t>(i);
            state = Expecting::NONE;
        }
        return true;
    }

    bool Uint64(uint64_t u) {
        if (state == Expecting::VALUE) {
            sum += u;
            state = Expecting::NONE;
        }
        return true;
    }

    bool Int64(int64_t i) {
        if (state == Expecting::VALUE) {
            sum += static_cast<uint64_t>(i);
            state = Expecting::NONE;
        }
        return true;
    }

    bool Key(const char* str, rapidjson::SizeType length, bool copy) {
        // We are looking for "id", "a", or "b".
        // "id" has length 2, "a" and "b" have length 1.
        if (length == 2) {
            if (str[0] == 'i' && str[1] == 'd') {
                state = Expecting::VALUE;
            } else {
                state = Expecting::NONE;
            }
        } else if (length == 1) {
            if (str[0] == 'a' || str[0] == 'b') {
                state = Expecting::VALUE;
            } else {
                state = Expecting::NONE;
            }
        } else {
            state = Expecting::NONE;
        }
        return true;
    }

    // Ignore other types to keep the parser moving fast
    bool Default() { 
        state = Expecting::NONE;
        return true; 
    }
};

} // namespace

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    ChecksumHandler handler;
    
    // Use kParseDefaultFlags for standard compliance; 
    // kParseInsituFlag is not applicable here as we use StringStream.
    reader.Parse(ss, handler);
    
    return handler.sum;
}