#include "interface.h"

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

std::string minify_json(const std::string& json_input) {
    // Use RapidJSON's SAX (Simple API for XML-like) parsing for maximum performance.
    // This avoids building a DOM tree and processes the JSON in a single pass.
    
    rapidjson::Reader reader;
    rapidjson::StringBuffer buffer;
    
    // Pre-allocate buffer capacity to avoid multiple reallocations.
    // Minified JSON is guaranteed to be smaller than or equal to the input size.
    buffer.Reserve(json_input.size());
    
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    
    // StringStream provides a wrapper around the input string for the Reader.
    rapidjson::StringStream ss(json_input.c_str());
    
    // Parse the input and pipe events directly to the writer.
    // The Reader preserves the order of keys as they appear in the stream.
    // kParseNoFlags is the fastest parsing mode.
    if (reader.Parse(ss, writer)) {
        return std::string(buffer.GetString(), buffer.GetSize());
    }
    
    // In case of a parse error, return an empty string or handle as needed.
    // Given the task context, we assume valid JSON input.
    return "";
}