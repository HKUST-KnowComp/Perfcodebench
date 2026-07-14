#include "interface.h"
#include "yyjson.h"
#include <string>
#include <vector>
#include <cstdint>

std::string build_json(const std::vector<uint32_t>& ids,
                       const std::vector<uint32_t>& values,
                       const std::vector<std::string>& names) {
    const size_t n = ids.size();
    if (n == 0) return "[]";

    // Pre-size buffer: rough upper bound
    size_t est = 2 + n * (8 + 12 + 12 + 32); // brackets + per-object
    std::string out;
    out.reserve(est);

    out.push_back('[');
    for (size_t i = 0; i < n; ++i) {
        if (i) out.push_back(',');
        out.append("{\"id\":");
        out.append(std::to_string(ids[i]));
        out.append(",\"flag\":");
        out.append((ids[i] % 3U == 0U) ? "true" : "false");
        out.append(",\"value\":");
        out.append(std::to_string(values[i]));
        out.append(",\"name\":\"");
        // escape names (only \" and \\ required for correctness)
        const std::string& name = names[i];
        for (char c : name) {
            if (c == '\\') { out.append("\\\\"); }
            else if (c == '"') { out.append("\\\""); }
            else { out.push_back(c); }
        }
        out.push_back('"');
        out.push_back('}');
    }
    out.push_back(']');
    return out;
}