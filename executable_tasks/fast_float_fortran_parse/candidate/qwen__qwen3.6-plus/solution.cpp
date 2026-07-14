#include "interface.h"
#include <fast_float/fast_float.h>
#include <string>
#include <system_error>

double parse_sum(const std::string& input) {
    const char* p = input.data();
    const char* end = p + input.size();
    double sum = 0.0;
    char buf[256];

    while (p < end) {
        while (p < end && (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')) {
            ++p;
        }
        if (p == end) break;

        const char* line_end = p;
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }

        size_t len = line_end - p;
        while (len > 0 && (p[len-1] == ' ' || p[len-1] == '\t' || p[len-1] == '\r')) {
            --len;
        }

        if (len == 0) {
            p = line_end + 1;
            continue;
        }

        if (len < sizeof(buf)) {
            for (size_t i = 0; i < len; ++i) {
                char c = p[i];
                buf[i] = (c == 'd' || c == 'D') ? 'e' : c;
            }
            double val;
            auto res = fast_float::from_chars(buf, buf + len, val);
            if (res.ec == std::errc()) {
                sum += val;
            }
        } else {
            std::string token(p, len);
            for (char& c : token) {
                if (c == 'd' || c == 'D') c = 'e';
            }
            double val;
            auto res = fast_float::from_chars(token.data(), token.data() + token.size(), val);
            if (res.ec == std::errc()) {
                sum += val;
            }
        }
        p = line_end + 1;
    }
    return sum;
}
