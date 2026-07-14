#include "interface.h"
#include "rapidjson/reader.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"

using namespace rapidjson;

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;
    
    // Use RapidJSON's Reader for a streaming approach to avoid DOM overhead
    // Since the task is to parse a large array of objects, we can use a custom handler
    struct ChecksumHandler : public BaseReader {
        uint64_t total_sum = 0;
        std::string current_key;
        bool in_nested = false;
        bool in_array = false;

        bool Key(const char* str, size_t length, bool copy) override {
            current_key.assign(str, length);
            if (current_key == "nested") {
                in_nested = true;
            }
            return true;
        }

        bool Uint(unsigned u) override {
            if (current_key == "id" && !in_nested) {
                total_sum += u;
            } else if (in_nested) {
                if (current_key == "a" || current_key == "b") {
                    total_sum += u;
                }
            }
            return true;
        }

        bool Int(int i) override {
            // Handle negative numbers if they appear, though task implies uint64_t
            uint64_t val = static_cast<uint64_t>(i);
            if (current_key == "id" && !in_nested) {
                total_sum += val;
            } else if (in_nested) {
                if (current_key == "a" || current_key == "b") {
                    total_sum += val;
                }
            }
            return true;
        }

        bool StartObject() override {
            return true;
        }

        bool EndObject(bool zeroRC) override {
            in_nested = false;
            return true;
        }

        bool StartArray() override {
            in_array = true;
            return true;
        }

        bool EndArray(bool zeroRC) override {
            in_array = false;
            return true;
        }
    };

    // For the specific structure of this task, using the Reader is fast, 
    // but since we have rapidjson/document.h available, we can use the 
    // more ergonomic but still fast Reader API or the Document if memory allows.
    // However, for "xlarge" workloads, the Reader is the gold standard.
    
    // To ensure correctness and simplicity while using the provided headers:
    Reader reader;
    ChecksumHandler handler;
    StringStream ss(json_input.c_str());
    reader.Read(ss, handler);
    
    return handler.total_sum;
}

// The above Reader implementation is a sketch. Let's provide a robust 
// implementation using the Document for simplicity if the memory limit allows,
// but since it's "xlarge", we use the Reader pattern correctly.

// Re-implementing with a more standard RapidJSON approach for the specific fields:

uint64_t compute_checksum_fast(const std::string& json_input) {
    uint64_t sum = 0;
    Document doc;
    // Use Insitu if possible, but json_input is const std::string
    if (doc.Parse(json_input.c_str()).HasParseError()) return 0;

    if (!doc.IsArray()) return 0;

    for (auto& item : doc.GetArray()) {
        if (item.HasMember("id") && item["id"].IsUint64()) {
            sum += item["id"].GetUint64();
        }
        if (item.HasMember("nested") && item["nested"].IsObject()) {
            const auto& nested = item["nested"];
            if (nested.HasMember("a") && nested["a"].IsUint64()) {
                sum += nested["a"].GetUint64();
            }
            if (nested.HasMember("b") && nested["b"].IsUint64()) {
                sum += nested["b"].GetUint64();
            }
        }
    }
    return sum;
}

// To truly optimize for "xlarge", we should use the Reader. 
// Let's provide the most performant version that fits the provided headers.

#include "rapidjson/reader.h"

struct ChecksumHandler : public Reader::Handler {
    uint64_t sum = 0;
    std::string key;
    bool in_nested = false;

    bool Key(const char* str, size_t length, bool copy) override {
        key.assign(str, length);
        in_nested = (key == "nested");
        return true;
    }
    bool Uint(unsigned u) override {
        if (key == "id" && !in_nested) sum += u;
        else if (in_nested && (key == "a" || key == "b")) sum += u;
        return true;
    }
    bool Int(int i) override {
        if (key == "id" && !in_nested) sum += (uint64_t)i;
        else if (in_nested && (key == "a" || key == "b")) sum += (uint64_t)i;
        return true;
    }
    bool StartObject() override { return true; }
    bool EndObject(bool) override { in_nested = false; return true; }
    bool StartArray() override { return true; }
    bool EndArray(bool) override { return true; }
    bool Null() override { return true; }
    bool Bool(bool) override { return true; }
    bool Double() override { return true; }
    bool String(const char*, size_t, bool) override { return true; }
};

uint64_t compute_checksum_final(const std::string& json_input) {
    ChecksumHandler handler;
    Reader reader;
    StringStream ss(json_input.c_str());
    reader.Read(ss, handler);
    return handler.sum;
}

// Final override to match the interface
uint64_t compute_checksum(const std::string& json_input) {
    return compute_checksum_final(json_input);
}