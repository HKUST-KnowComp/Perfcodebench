#include "interface.h"

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace {

// Fast uint32 to string, returns pointer past last digit
inline char* u32toa(uint32_t v, char* buf) {
    char tmp[10];
    char* p = tmp + 10;
    do {
        *--p = '0' + (v % 10);
        v /= 10;
    } while (v);
    while (p < tmp + 10) *buf++ = *p++;
    return buf;
}

// Escape JSON string directly into StringBuffer
inline void write_escaped_string(rapidjson::StringBuffer& sb, const std::string& s) {
    sb.Put('"');
    for (unsigned char c : s) {
        switch (c) {
            case '"':  sb.Put('\\'); sb.Put('"'); break;
            case '\\': sb.Put('\\'); sb.Put('\\'); break;
            case '\b': sb.Put('\\'); sb.Put('b'); break;
            case '\f': sb.Put('\\'); sb.Put('f'); break;
            case '\n': sb.Put('\\'); sb.Put('n'); break;
            case '\r': sb.Put('\\'); sb.Put('r'); break;
            case '\t': sb.Put('\\'); sb.Put('t'); break;
            default:
                if (c < 0x20) {
                    // Control character: \u00XX
                    sb.Put('\\'); sb.Put('u'); sb.Put('0'); sb.Put('0');
                    static const char hex[] = "0123456789abcdef";
                    sb.Put(hex[c >> 4]);
                    sb.Put(hex[c & 0xF]);
                } else {
                    sb.Put(static_cast<char>(c));
                }
        }
    }
    sb.Put('"');
}

} // namespace

std::string build_json(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& values,
    const std::vector<std::string>& names) {
    
    const size_t n = ids.size();
    
    // Estimate capacity: each record ~50 bytes base + name length
    size_t est = 2; // []
    for (size_t i = 0; i < n; ++i) {
        est += 60 + names[i].size();
    }
    
    rapidjson::StringBuffer sb;
    sb.Reserve(est);
    
    sb.Put('[');
    
    char numbuf[12];
    
    for (size_t i = 0; i < n; ++i) {
        if (i > 0) sb.Put(',');
        
        // {"id":
        sb.Put('{');
        sb.Put('"'); sb.Put('i'); sb.Put('d'); sb.Put('"'); sb.Put(':');
        
        // id value
        char* end = u32toa(ids[i], numbuf);
        for (char* p = numbuf; p < end; ++p) sb.Put(*p);
        
        // ,"flag":
        sb.Put(',');
        sb.Put('"'); sb.Put('f'); sb.Put('l'); sb.Put('a'); sb.Put('g'); sb.Put('"'); sb.Put(':');
        
        // flag value
        if ((ids[i] % 3U) == 0U) {
            sb.Put('t'); sb.Put('r'); sb.Put('u'); sb.Put('e');
        } else {
            sb.Put('f'); sb.Put('a'); sb.Put('l'); sb.Put('s'); sb.Put('e');
        }
        
        // ,"value":
        sb.Put(',');
        sb.Put('"'); sb.Put('v'); sb.Put('a'); sb.Put('l'); sb.Put('u'); sb.Put('e'); sb.Put('"'); sb.Put(':');
        
        // value
        end = u32toa(values[i], numbuf);
        for (char* p = numbuf; p < end; ++p) sb.Put(*p);
        
        // ,"name":
        sb.Put(',');
        sb.Put('"'); sb.Put('n'); sb.Put('a'); sb.Put('m'); sb.Put('e'); sb.Put('"'); sb.Put(':');
        
        // name string (escaped)
        write_escaped_string(sb, names[i]);
        
        sb.Put('}');
    }
    
    sb.Put(']');
    
    return std::string(sb.GetString(), sb.GetSize());
}