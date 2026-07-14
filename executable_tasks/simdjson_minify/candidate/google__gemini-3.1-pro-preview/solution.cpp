#include "interface.h"
#include "simdjson.h"
#include <string>

/**
 * Optimized JSON minification using simdjson.
 * 
 * simdjson's DOM parser is extremely fast and uses a tape-based representation
 * that preserves the original order of fields in the JSON document. 
 * The simdjson::minify function efficiently serializes this DOM back to a 
 * string without whitespace.
 */
std::string minify_json(const std::string& json_input) {
    // Reusable parser instance. For a single-call interface, we create it here.
    // In a high-throughput environment, this could be thread-local.
    simdjson::dom::parser parser;
    
    // Parse the input string into a DOM element.
    // simdjson::dom::element preserves field order as it appears in the source.
    simdjson::dom::element doc;
    auto error = parser.parse(json_input).get(doc);
    
    if (error) {
        // If parsing fails, the behavior isn't strictly defined by the task,
        // but returning an empty string or the original is a safe fallback.
        return "";
    }

    // Use simdjson's optimized minification utility.
    // This returns a std::string containing the minified JSON.
    return simdjson::minify(doc);
}
