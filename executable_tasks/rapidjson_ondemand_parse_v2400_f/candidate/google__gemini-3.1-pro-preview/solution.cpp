#include "interface.h"
#include <rapidjson/reader.h>
#include <string>
#include <cstdint>

using namespace rapidjson;

struct ChecksumHandler {
    uint64_t sum = 0;
    int depth = 0;
    
    enum class KeyType { OTHER, ID, NESTED, A, B };
    KeyType current_key = KeyType::OTHER;

    // Handle numeric values for the target keys
    bool Uint64(uint64_t i) {
        if (depth == 1 && current_key == KeyType::ID) {
            sum += i;
        } else if (depth == 2 && (current_key == KeyType::A || current_key == KeyType::B)) {
            sum += i;
        }
        return true;
    }

    bool Uint(unsigned i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Int(int i) { return Uint64(static_cast<uint64_t>(i)); }
    bool Int64(int64_t i) { return Uint64(static_cast<uint64_t>(i)); }

    bool Key(const char* str, SizeType length, bool copy) {
        if (depth == 1) {
            if (length == 2 && str[0] == 'i' && str[1] == 'd') {
                current_key = KeyType::ID;
            } else if (length == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' && str[3] == 't' && str[4] == 'e' && str[5] == 'd') {
                current_key = KeyType::NESTED;
            } else {
                current_key = KeyType::OTHER;
            }
        } else if (depth == 2) {
            if (length == 1) {
                if (str[0] == 'a') current_key = KeyType::A;
                else if (str[0] == 'b') current_key = KeyType::B;
                else current_key = KeyType::OTHER;
            } else {
                current_key = KeyType::OTHER;
            }
        } else {
            current_key = KeyType::OTHER;
        }
        return true;
    }

    bool StartObject() {
        depth++;
        return true;
    }

    bool EndObject(SizeType memberCount) {
        depth--;
        return true;
    }

    // Required SAX interface stubs
    bool Null() { return true; }
    bool Bool(bool b) { return true; }
    bool Double(double d) { return true; }
    bool RawNumber(const char* str, SizeType length, bool copy) { return true; }
    bool String(const char* str, SizeType length, bool copy) { return true; }
    bool StartArray() { return true; }
    bool EndArray(SizeType elementCount) { return true; }
};

uint64_t compute_checksum(const std::string& json_input) {
    ChecksumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    // Use kParseNoFlags for maximum speed; assumes valid JSON input per task contract
    reader.Parse<kParseNoFlags>(ss, handler);
    return handler.sum;
}