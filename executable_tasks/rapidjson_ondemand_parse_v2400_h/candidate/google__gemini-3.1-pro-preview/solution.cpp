#include "interface.h"
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>

using namespace rapidjson;

struct ChecksumHandler : public BaseReaderHandler<UTF8<>, ChecksumHandler> {
    uint64_t sum = 0;
    
    enum State {
        ROOT,
        IN_ARRAY,
        IN_OBJECT,
        IN_NESTED
    };

    enum Expecting {
        NOTHING,
        ID,
        NESTED,
        A,
        B
    };

    State state = ROOT;
    Expecting expecting = NOTHING;

    bool StartArray() {
        if (state == ROOT) {
            state = IN_ARRAY;
            return true;
        }
        return true;
    }

    bool EndArray(SizeType) {
        return true;
    }

    bool StartObject() {
        if (state == IN_ARRAY) {
            state = IN_OBJECT;
        } else if (state == IN_OBJECT && expecting == NESTED) {
            state = IN_NESTED;
        }
        return true;
    }

    bool EndObject(SizeType) {
        if (state == IN_NESTED) {
            state = IN_OBJECT;
            expecting = NOTHING;
        } else if (state == IN_OBJECT) {
            state = IN_ARRAY;
            expecting = NOTHING;
        }
        return true;
    }

    bool Key(const char* str, SizeType length, bool) {
        if (state == IN_OBJECT) {
            if (length == 2 && str[0] == 'i' && str[1] == 'd') {
                expecting = ID;
            } else if (length == 6 && str[0] == 'n' && str[1] == 'e' && str[2] == 's' && str[3] == 't' && str[4] == 'e' && str[5] == 'd') {
                expecting = NESTED;
            } else {
                expecting = NOTHING;
            }
        } else if (state == IN_NESTED) {
            if (length == 1) {
                if (str[0] == 'a') expecting = A;
                else if (str[0] == 'b') expecting = B;
                else expecting = NOTHING;
            } else {
                expecting = NOTHING;
            }
        }
        return true;
    }

    bool Uint(unsigned i) {
        if (expecting == ID || expecting == A || expecting == B) {
            sum += i;
        }
        return true;
    }

    bool Uint64(uint64_t i) {
        if (expecting == ID || expecting == A || expecting == B) {
            sum += i;
        }
        return true;
    }

    bool Int(int i) {
        if (i >= 0 && (expecting == ID || expecting == A || expecting == B)) {
            sum += static_cast<uint64_t>(i);
        }
        return true;
    }

    bool Int64(int64_t i) {
        if (i >= 0 && (expecting == ID || expecting == A || expecting == B)) {
            sum += static_cast<uint64_t>(i);
        }
        return true;
    }
};

uint64_t compute_checksum(const std::string& json_input) {
    Reader reader;
    StringStream ss(json_input.c_str());
    ChecksumHandler handler;
    reader.Parse(ss, handler);
    return handler.sum;
}