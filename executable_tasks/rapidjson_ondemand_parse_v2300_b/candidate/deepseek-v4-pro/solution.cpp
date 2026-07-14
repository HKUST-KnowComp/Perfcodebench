#include "interface.h"

#include <rapidjson/document.h>
#include <vector>

uint64_t compute_checksum(const std::string& json_input) {
    // Copy to mutable buffer for in-situ parsing
    std::vector<char> buffer(json_input.begin(), json_input.end());
    buffer.push_back('\0'); // null-terminate

    rapidjson::Document doc;
    // Parse in-situ: modifies buffer, fast creation of strings as pointers into buffer
    doc.ParseInsitu(buffer.data());

    // Assume valid JSON; if parse error, return 0 (should never happen in benchmark)
    if (doc.HasParseError()) return 0;

    uint64_t sum = 0;

    // top-level is an array
    for (rapidjson::SizeType i = 0; i < doc.Size(); ++i) {
        const auto& item = doc[i];
        // Access "id" directly
        sum += item["id"].GetUint64();
        // Access nested object
        const auto& nested = item["nested"];
        sum += nested["a"].GetUint64();
        sum += nested["b"].GetUint64();
    }

    return sum;
}
