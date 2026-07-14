#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    bool in_array = false;
    bool in_nested = false;
    std::string current_key;
    int depth = 0;

    bool StartArray() {
        depth++;
        if (depth == 1) in_array = true;
        return true;
    }

    bool EndArray() {
        if (depth == 1) in_array = false;
        depth--;
        return true;
    }

    bool StartObject() {
        depth++;
        return true;
    }

    bool EndObject() {
        if (in_nested) in_nested = false;
        depth--;
        return true;
    }

    bool Key(const char* str, SizeType length, bool copy) {
        current_key.assign(str, length);
        return true;
    }

    bool Uint64(uint64_t i) {
        if (in_array) {
            if (current_key == "id") {
                sum += i;
            } else if (in_nested) {
                if (current_key == "a" || current_key == "b") {
                    sum += i;
                }
            }
        }
        return true;
    }

    bool Int64(int64_t i) {
        return Uint64(static_cast<uint64_t>(i));
    }

    bool Double(double d) {
        // The task implies uint64_t fields, but we handle numeric types generally
        return true;
    }

    // Handle nested logic via key tracking
    bool StartObject() {
        depth++;
        if (current_key == "nested") {
            in_nested = true;
        }
        return true;
    }

    // Override to handle the specific structure logic
    bool Key(const char* str, SizeType length, bool copy) {
        current_key.assign(str, length);
        return true;
    }

    // Re-implementing logic to be more robust for the specific schema
    // We need to know if we are inside the 'nested' object
    bool StartObject(SizeType elementCount = 0) {
        depth++;
        if (current_key == "nested") {
            in_nested = true;
        }
        return true;
    }

    bool EndObject(SizeType elementCount = 0) {
        if (in_nested && depth > 1) {
            // Check if the parent was 'nested'
            // In a real SAX parser, we'd use a stack, but for this specific schema:
            // we can track if we just exited a 'nested' object.
            // However, a simpler way is to track depth and key.
        }
        // Simplified for the specific schema: 
        // We'll use a small stack or state machine.
        return true;
    }
};

// Refined Handler for the specific schema: [ {"id":..., "nested": {"a":..., "b":...}}, ... ]
struct OptimizedHandler : public BaseReaderHandler<UTF8<>, OptimizedHandler> {
    uint64_t sum = 0;
    enum State { ROOT, ARRAY, OBJECT, NESTED_OBJECT };
    State state = ROOT;
    std::string last_key;

    bool StartArray() { state = ARRAY; return true; }
    bool EndArray() { state = ROOT; return true; }
    bool StartObject() {
        if (state == ARRAY) state = OBJECT;
        else if (state == OBJECT && last_key == "nested") state = NESTED_OBJECT;
        return true;
    }
    bool EndObject() {
        if (state == NESTED_OBJECT) state = OBJECT;
        else if (state == OBJECT) state = ARRAY;
        return true;
    }
    bool Key(const char* str, SizeType length, bool copy) {
        last_key.assign(str, length);
        return true;
    }
    bool Uint64(uint64_t i) {
        if (state == OBJECT && last_key == "id") sum += i;
        else if (state == NESTED_OBJECT && (last_key == "a" || last_key == "b")) sum += i;
        return true;
    }
    bool Int64(int64_t i) { return Uint64(static_cast<uint64_t>(i)); }
};

uint64_t compute_checksum(const std::string& json_input) {
    Reader reader;
    StringStream ss(json_input.c_str());
    OptimizedHandler handler;
    reader.Parse(ss, handler);
    return handler.sum;
}