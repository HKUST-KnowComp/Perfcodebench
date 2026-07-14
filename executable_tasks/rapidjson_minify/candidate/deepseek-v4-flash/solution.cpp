#include "interface.h"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/reader.h>
#include <string>

class MinifyHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>> {
public:
    explicit MinifyHandler(rapidjson::Writer<rapidjson::StringBuffer>& writer) : writer_(writer) {}
    bool Null() { writer_.Null(); return true; }
    bool Bool(bool b) { writer_.Bool(b); return true; }
    bool Int(int i) { writer_.Int(i); return true; }
    bool Uint(unsigned u) { writer_.Uint(u); return true; }
    bool Int64(int64_t i) { writer_.Int64(i); return true; }
    bool Uint64(uint64_t u) { writer_.Uint64(u); return true; }
    bool Double(double d) { writer_.Double(d); return true; }
    bool String(const Ch* str, SizeType length, bool copy) { writer_.String(str, length); return true; }
    bool StartObject() { writer_.StartObject(); return true; }
    bool Key(const Ch* str, SizeType length, bool copy) { writer_.Key(str, length); return true; }
    bool EndObject(SizeType memberCount) { writer_.EndObject(); return true; }
    bool StartArray() { writer_.StartArray(); return true; }
    bool EndArray(SizeType elementCount) { writer_.EndArray(); return true; }
private:
    rapidjson::Writer<rapidjson::StringBuffer>& writer_;
};

std::string minify_json(const std::string& json_input) {
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    MinifyHandler handler(writer);
    rapidjson::Reader reader;
    rapidjson::StringStream ss(json_input.c_str());
    reader.Parse(ss, handler);
    return sb.GetString();
}