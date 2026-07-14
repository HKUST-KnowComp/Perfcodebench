#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
  uint64_t hash = 0;
  const std::size_t codes_size = codes.size();
  const uint32_t* codes_ptr = codes.data();
  
  // Precompute dictionary data pointers for faster access
  std::vector<const char*> dict_data(dict.size());
  std::vector<std::size_t> dict_sizes(dict.size());
  for (std::size_t i = 0; i < dict.size(); ++i) {
    dict_data[i] = dict[i].data();
    dict_sizes[i] = dict[i].size();
  }
  
  const char** dict_data_ptr = dict_data.data();
  const std::size_t* dict_sizes_ptr = dict_sizes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    std::size_t i = 0;
    // Unroll by 4 for better ILP
    for (; i + 3 < codes_size; i += 4) {
      uint32_t code0 = codes_ptr[i];
      uint32_t code1 = codes_ptr[i + 1];
      uint32_t code2 = codes_ptr[i + 2];
      uint32_t code3 = codes_ptr[i + 3];
      
      std::size_t size0 = dict_sizes_ptr[code0];
      std::size_t size1 = dict_sizes_ptr[code1];
      std::size_t size2 = dict_sizes_ptr[code2];
      std::size_t size3 = dict_sizes_ptr[code3];
      
      const char* data0 = dict_data_ptr[code0];
      const char* data1 = dict_data_ptr[code1];
      const char* data2 = dict_data_ptr[code2];
      const char* data3 = dict_data_ptr[code3];
      
      hash = mix(hash, size0);
      hash = mix(hash, size0 ? static_cast<unsigned char>(data0[0]) : 0u);
      
      hash = mix(hash, size1);
      hash = mix(hash, size1 ? static_cast<unsigned char>(data1[0]) : 0u);
      
      hash = mix(hash, size2);
      hash = mix(hash, size2 ? static_cast<unsigned char>(data2[0]) : 0u);
      
      hash = mix(hash, size3);
      hash = mix(hash, size3 ? static_cast<unsigned char>(data3[0]) : 0u);
    }
    
    // Handle remaining elements
    for (; i < codes_size; ++i) {
      uint32_t code = codes_ptr[i];
      std::size_t size = dict_sizes_ptr[code];
      const char* data = dict_data_ptr[code];
      
      hash = mix(hash, size);
      hash = mix(hash, size ? static_cast<unsigned char>(data[0]) : 0u);
    }
  }
  
  return hash;
}