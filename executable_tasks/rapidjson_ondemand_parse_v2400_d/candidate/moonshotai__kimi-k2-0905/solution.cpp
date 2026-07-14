#include "interface.h"

#include <rapidjson/reader.h>
#include <rapidjson/stream.h>
#include <cstdint>

using namespace rapidjson;

class ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
public:
    ChecksumHandler() : state_(ExpectArrayStart), sum_(0) {}

    bool Uint64(uint64_t u) {
        switch (state_) {
            case ExpectId:      sum_ += u; state_ = ExpectCommaAfterId; return true;
            case ExpectNestedA: sum_ += u; state_ = ExpectCommaAfterA; return true;
            case ExpectNestedB: sum_ += u; state_ = ExpectCommaAfterB; return true;
            default: return true;
        }
    }

    bool StartArray()   { return state_ == ExpectArrayStart; }
    bool EndArray(SizeType) { return true; }

    bool StartObject() {
        switch (state_) {
            case ExpectObjectStart:
            case ExpectCommaAfterObject:
                state_ = ExpectKey;
                return true;
            case ExpectNestedObject:
                state_ = ExpectNestedKey;
                return true;
            default:
                return true;
        }
    }

    bool EndObject(SizeType) {
        switch (state_) {
            case ExpectCommaAfterB:
                state_ = ExpectCommaAfterObject;
                return true;
            default:
                return true;
        }
    }

    bool Key(const char* str, SizeType len, bool) {
        switch (state_) {
            case ExpectKey:
                if (len == 2 && std::memcmp(str, "id", 2) == 0) {
                    state_ = ExpectColonAfterKey;
                } else if (len == 6 && std::memcmp(str, "nested", 6) == 0) {
                    state_ = ExpectColonAfterNested;
                }
                return true;
            case ExpectNestedKey:
                if (len == 1 && str[0] == 'a') {
                    state_ = ExpectColonAfterNestedA;
                } else if (len == 1 && str[0] == 'b') {
                    state_ = ExpectColonAfterNestedB;
                }
                return true;
            default:
                return true;
        }
    }

    bool Default() { return true; }

    uint64_t checksum() const { return sum_; }

private:
    enum State {
        ExpectArrayStart,
        ExpectObjectStart,
        ExpectKey,
        ExpectColonAfterKey,
        ExpectId,
        ExpectCommaAfterId,
        ExpectColonAfterNested,
        ExpectNestedObject,
        ExpectNestedKey,
        ExpectColonAfterNestedA,
        ExpectNestedA,
        ExpectCommaAfterA,
        ExpectColonAfterNestedB,
        ExpectNestedB,
        ExpectCommaAfterB,
        ExpectCommaAfterObject
    } state_;
    uint64_t sum_;
};

uint64_t compute_checksum(const std::string& json_input) {
    StringStream ss(json_input.c_str());
    ChecksumHandler handler;
    Reader reader;
    reader.Parse(ss, handler);
    return handler.checksum();
}