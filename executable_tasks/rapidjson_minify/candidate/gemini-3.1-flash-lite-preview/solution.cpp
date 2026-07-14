#include "interface.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;

std::string minify_json(const std::string& json_input) {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);
    Reader reader;
    StringStream ss(json_input.c_str());

    if (reader.Parse(ss, writer)) {
        return std::string(sb.GetString(), sb.GetSize());
    }
    return "";
}