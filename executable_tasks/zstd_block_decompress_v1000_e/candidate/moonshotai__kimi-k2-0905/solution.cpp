#include "interface.h"
#include "zstd.h"
#include <string>
#include <cstddef>
#include <cstdint>

uint64_t decompress_checksum(const std::string& compressed,std::size_t decompressed_size) {
    std::string restored(decompressed_size,char());
    size_t r=ZSTD_decompress(restored.data(),decompressed_size,
                            compressed.data(),compressed.size());
    if(ZSTD_isError(r)||r!=decompressed_size) return 0;

    const unsigned char* p=reinterpret_cast<const unsigned char*>(restored.data());
    const unsigned char* end=p+decompressed_size;
    uint64_t h0=14695981039346656037ULL,h1=1099511628211ULL;

    while(end-p>=4){
        h0^=p[0]; h0*=h1;
        h0^=p[1]; h0*=h1;
        h0^=p[2]; h0*=h1;
        h0^=p[3]; h0*=h1;
        p+=4;
    }
    while(p!=end){
        h0^=*p++; h0*=h1;
    }
    return h0;
}