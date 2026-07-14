#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;

struct ChecksumHandler {
    uint64_t sum = 0;
    enum KeyType { kOther, kId, kA, kB };
    KeyType current_key = kOther;

    // Handle unsigned integers (RapidJSON distinguishes between 32-bit and 64-bit)
    bool Uint(unsigned i) {
        if (current_key != kOther) sum += i;
        return true;
    }
    bool Uint64(uint64_t i) {
        if (current_key != kOther) sum += i;
        return true;
    }

    // Handle signed integers just in case, though checksum expects unsigned
    bool Int(int i) {
        if (current_key != kOther) sum += static_cast<uint64_t>(i);
        return true;
    }
    bool Int64(int64_t i) {
        if (current_key != kOther) sum += static_cast<uint64_t>(i);
        return true;
    }

    bool Key(const char* str, SizeType length, bool copy) {
        if (length == 2 && str[0] == 'i' && str[1] == 'd') {
            current_key = kId;
        } else if (length == 1) {
            if (str[0] == 'a') current_key = kA;
            else if (str[0] == 'b') current_key = kB;
            else current_key = kOther;
        } else {
            current_key = kOther;
        }
        return true;
    }

    // Required SAX interface methods
    bool Null() { return true; }
    bool Bool(bool b) { return true; }
    bool Double(double d) { return true; }
    bool RawNumber(const char* str, SizeType length, bool copy) { return true; }
    bool String(const char* str, SizeType length, bool copy) { return true; }
    bool StartObject() { return true; }
    bool EndObject(SizeType memberCount) { return true; }
    bool StartArray() { return true; }
    bool EndArray(SizeType elementCount) { return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    Reader reader;
    StringStream ss(json_input.c_str());
    ChecksumHandler handler;
    
    // Use kParseDefaultFlags for standard parsing
    reader.Parse(ss, handler);
    
    return handler.sum;
}