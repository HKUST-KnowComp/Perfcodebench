#include "interface.h"
#include "fast_float/fast_float.h"
#include <system_error>

double parse_sum(const std::string& input) {
    std::string buf = input;
    for (char& c : buf) {
        if (c == 'd' || c == 'D') c = 'e';
    }
    
    const char* p = buf.data();
    const char* end = p + buf.size();
    double sum = 0.0;
    
    while (p < end) {
        while (p < end && (*p == '\n' || *p == '\r' || *p == ' ' || *p == '\t')) {
            ++p;
        }
        if (p >= end) break;
        
        double val = 0.0;
        auto res = fast_float::from_chars(p, end, val);
        if (res.ec != std::errc{}) {
            break;
        }
        sum += val;
        p = res.ptr;
    }
    return sum;
}
