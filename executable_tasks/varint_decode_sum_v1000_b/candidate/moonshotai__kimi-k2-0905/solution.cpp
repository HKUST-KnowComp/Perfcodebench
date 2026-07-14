#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
    const uint8_t* data = encoded.data();
    const std::size_t len = encoded.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint8_t* ptr = data;
        const uint8_t* end = data + len;
        uint64_t acc = 0;

        while (ptr + 10 <= end) {
            uint64_t val;
            uint64_t byte0 = ptr[0];
            if (!(byte0 & 0x80u)) {
                val = byte0;
                ptr += 1;
            } else {
                uint64_t byte1 = ptr[1];
                if (!(byte1 & 0x80u)) {
                    val = ((byte0 & 0x7Fu)      ) | (byte1 << 7);
                    ptr += 2;
                } else {
                    uint64_t byte2 = ptr[2];
                    if (!(byte2 & 0x80u)) {
                        val = ((byte0 & 0x7Fu)      ) |
                              ((byte1 & 0x7Fu) << 7 ) |
                              ( byte2         << 14);
                        ptr += 3;
                    } else {
                        uint64_t byte3 = ptr[3];
                        if (!(byte3 & 0x80u)) {
                            val = ((byte0 & 0x7Fu)      ) |
                                  ((byte1 & 0x7Fu) << 7 ) |
                                  ((byte2 & 0x7Fu) << 14) |
                                  ( byte3         << 21);
                            ptr += 4;
                        } else {
                            uint64_t byte4 = ptr[4];
                            if (!(byte4 & 0x80u)) {
                                val = ((byte0 & 0x7Fu)      ) |
                                      ((byte1 & 0x7Fu) << 7 ) |
                                      ((byte2 & 0x7Fu) << 14) |
                                      ((byte3 & 0x7Fu) << 21) |
                                      ( byte4         << 28);
                                ptr += 5;
                            } else {
                                uint64_t byte5 = ptr[5];
                                if (!(byte5 & 0x80u)) {
                                    val = ((byte0 & 0x7Fu)      ) |
                                          ((byte1 & 0x7Fu) << 7 ) |
                                          ((byte2 & 0x7Fu) << 14) |
                                          ((byte3 & 0x7Fu) << 21) |
                                          ((byte4 & 0x7Fu) << 28) |
                                          ( byte5         << 35);
                                    ptr += 6;
                                } else {
                                    uint64_t byte6 = ptr[6];
                                    if (!(byte6 & 0x80u)) {
                                        val = ((byte0 & 0x7Fu)      ) |
                                              ((byte1 & 0x7Fu) << 7 ) |
                                              ((byte2 & 0x7Fu) << 14) |
                                              ((byte3 & 0x7Fu) << 21) |
                                              ((byte4 & 0x7Fu) << 28) |
                                              ((byte5 & 0x7Fu) << 35) |
                                              ( byte6         << 42);
                                        ptr += 7;
                                    } else {
                                        uint64_t byte7 = ptr[7];
                                        if (!(byte7 & 0x80u)) {
                                            val = ((byte0 & 0x7Fu)      ) |
                                                  ((byte1 & 0x7Fu) << 7 ) |
                                                  ((byte2 & 0x7Fu) << 14) |
                                                  ((byte3 & 0x7Fu) << 21) |
                                                  ((byte4 & 0x7Fu) << 28) |
                                                  ((byte5 & 0x7Fu) << 35) |
                                                  ((byte6 & 0x7Fu) << 42) |
                                                  ( byte7         << 49);
                                            ptr += 8;
                                        } else {
                                            uint64_t byte8 = ptr[8];
                                            if (!(byte8 & 0x80u)) {
                                                val = ((byte0 & 0x7Fu)      ) |
                                                      ((byte1 & 0x7Fu) << 7 ) |
                                                      ((byte2 & 0x7Fu) << 14) |
                                                      ((byte3 & 0x7Fu) << 21) |
                                                      ((byte4 & 0x7Fu) << 28) |
                                                      ((byte5 & 0x7Fu) << 35) |
                                                      ((byte6 & 0x7Fu) << 42) |
                                                      ((byte7 & 0x7Fu) << 49) |
                                                      ( byte8         << 56);
                                                ptr += 9;
                                            } else {
                                                uint64_t byte9 = ptr[9];
                                                val = ((byte0 & 0x7Fu)      ) |
                                                      ((byte1 & 0x7Fu) << 7 ) |
                                                      ((byte2 & 0x7Fu) << 14) |
                                                      ((byte3 & 0x7Fu) << 21) |
                                                      ((byte4 & 0x7Fu) << 28) |
                                                      ((byte5 & 0x7Fu) << 35) |
                                                      ((byte6 & 0x7Fu) << 42) |
                                                      ((byte7 & 0x7Fu) << 49) |
                                                      ((byte8 & 0x7Fu) << 56) |
                                                      ( byte9         << 63);
                                                ptr += 10;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            acc += val;
        }

        while (ptr < end) {
            uint64_t value = 0;
            unsigned shift = 0;
            for (;;) {
                uint64_t byte = *ptr++;
                value |= (byte & 0x7Fu) << shift;
                if (!(byte & 0x80u)) break;
                shift += 7;
            }
            acc += value;
        }
        total = acc;
    }
    return total;
}