#include "interface.h"
#include <cstdint>
#include <string>
#include <cstddef>
#include <cstring>
#include <cassert>
#define YYJSON_IMPLEMENTATION // or include precompiled? but we include header only
#include "yyjson.h"

uint64_t compute_checksum(const std::string& json_input) {
    uint64_t sum = 0;
    yyjson_reader reader;
    yyjson_read_err err;
    yyjson_reader_init(&reader, json_input.data(), json_input.size(), 0);
    
    yyjson_val val;
    // Expect top-level array
    yyjson_reader_next(&reader, &val);
    assert(val.type == YYJSON_TYPE_ARR);
    
    while (true) {
        yyjson_reader_next(&reader, &val);
        if (val.type == YYJSON_TYPE_ARR_END) break;
        assert(val.type == YYJSON_TYPE_OBJ);
        
        uint64_t id_val = 0, a_val = 0, b_val = 0;
        
        // Read object pairs
        while (true) {
            yyjson_reader_next(&reader, &val);
            if (val.type == YYJSON_TYPE_OBJ_END) break;
            
            // val is key (string)
            const char* key = val.u.s;
            size_t key_len = val.len;
            yyjson_reader_next(&reader, &val);  // value
            
            if (key_len == 2 && key[0] == 'i' && key[1] == 'd') {
                id_val = yyjson_get_uint(&val);
            } else if (key_len == 6 &&
                       key[0] == 'n' && key[1] == 'e' && key[2] == 's' &&
                       key[3] == 't' && key[4] == 'e' && key[5] == 'd') {
                // Nested object start
                yyjson_reader_next(&reader, &val);
                assert(val.type == YYJSON_TYPE_OBJ);
                while (true) {
                    yyjson_reader_next(&reader, &val);
                    if (val.type == YYJSON_TYPE_OBJ_END) break;
                    const char* nkey = val.u.s;
                    size_t nkey_len = val.len;
                    yyjson_reader_next(&reader, &val);
                    if (nkey_len == 1 && nkey[0] == 'a') {
                        a_val = yyjson_get_uint(&val);
                    } else if (nkey_len == 1 && nkey[0] == 'b') {
                        b_val = yyjson_get_uint(&val);
                    }
                }
            }
        }
        sum += id_val + a_val + b_val;
    }
    
    yyjson_reader_deinit(&reader);
    return sum;
}
