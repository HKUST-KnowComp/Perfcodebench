#pragma once

#include "interface.h"

#include <cstdint>
#include <string>
#include <cstring>
#include <rapidjson/document.h>

class ChecksumHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ChecksumHandler> {
public:
    ChecksumHandler() : nested_active_(false), last_key_(nullptr) {}

    bool Key(const char* str, rapidjson::SizeType length, bool copy) {
        last_key_ = str;
        return true;
    }

    bool Uint64(uint64_t u) {
        if (last_key_) {
            if (strcmp(last_key_, "id") == 0) {
                sum_ += u;
            } else if (nested_active_) {
                if (strcmp(last_key_, "a") == 0) sum_ += u;
                else if (strcmp(last_key_, "b") == 0) sum_ += u;
            }
        }
        return true;
    }

    bool Int64(int64_t i) {
        if (last_key_) {
            if (strcmp(last_key_, "id") == 0) {
                sum_ += static_cast<uint64_t>(i);
            } else if (nested_active_) {
                if (strcmp(last_key_, "a") == 0) sum_ += static_cast<uint64_t>(i);
                else if (strcmp(last_key_, "b") == 0) sum_ += static_cast<uint64_t>(i);
            }
        }
        return true;
    }

    bool StartObject() {
        if (last_key_ && strcmp(last_key_, "nested") == 0) {
            nested_active_ = true;
        }
        return true;
    }

    bool EndObject(rapidjson::SizeType memberCount) {
        if (nested_active_) {
            nested_active_ = false;
        }
        return true;
    }

    bool StartArray() {
        nested_active_ = false;
        last_key_ = nullptr;
        return true;
    }

    bool EndArray(rapidjson::SizeType elementCount) {
        return true;
    }

    bool Null() { return true; }
    bool Bool(bool b) { return true; }
    bool Double(double d) { return true; }
    bool String(const char* str, rapidjson::SizeType length, bool copy) { return true; }
    bool RawNumber(const char* str, rapidjson::SizeType length, bool copy, rapidjson::Type type) { return true; }

    uint64_t GetSum() const { return sum_; }

private:
    uint64_t sum_ = 0;
    bool nested_active_;
    const char* last_key_;
};

uint64_t compute_checksum(const std::string& json_input) {
    rapidjson::StringStream stream(json_input.c_str());
    rapidjson::Reader reader;
    ChecksumHandler handler;
    reader.Parse<0>(stream, handler);
    return handler.GetSum();
}