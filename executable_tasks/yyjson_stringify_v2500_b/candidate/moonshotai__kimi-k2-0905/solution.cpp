#include "interface.h"
#include <yyjson.h>
#include <string>
#include <vector>
#include <cstdint>

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names)
{
    const size_t n = ids.size();

    // Pre-calculate exact output length
    size_t len = 1; // '['
    for (size_t i = 0; i < n; ++i) {
        // {"id":<ids[i]>,"flag":<true|false>,"value":<values[i]>,"name":"<names[i]>"}
        len += 29;                            // fixed chars: {"id":,"flag":,"value":,"name":""}
        len += (ids[i] < 10) ? 1 : (ids[i] < 100) ? 2 : (ids[i] < 1000) ? 3 : 10;
        len += 4 + (i == 0 ? 0 : 1);          // "flag":true/false  + comma before object
        len += (values[i] < 10) ? 1 : (values[i] < 100) ? 2 : (values[i] < 1000) ? 3 : 10;
        len += names[i].size();               // name string content
    }
    if (n) len -= 1; // remove last comma
    len += 1;        // ']'

    std::string out;
    out.reserve(len);
    out.push_back('[');

    for (size_t i = 0; i < n; ++i) {
        if (i) out.push_back(',');
        out.append("{\"id\":");
        out.append(std::to_string(ids[i]));
        out.append(",\"flag\":");
        out.append((ids[i] % 3U) == 0U ? "true" : "false");
        out.append(",\"value\":");
        out.append(std::to_string(values[i]));
        out.append(",\"name\":\"");
        // names[i] is already valid UTF-8, no escaping needed per task rules
        out.append(names[i]);
        out.push_back('"');
        out.push_back('}');
    }

    out.push_back(']');
    return out;
}