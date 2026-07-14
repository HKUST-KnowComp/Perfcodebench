#include "is_utf8.h"
#include <cstddef>
#include <string>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const unsigned char* const end = p + input.size();
  
  while (p < end) {
    const unsigned char* line_start = p;
    
    // Single pass: find newline while validating UTF-8
    while (p < end) {
      const unsigned char c = *p;
      
      if (c == '\n') {
        // Validate line from line_start to p
        const unsigned char* line_end = p;
        const unsigned char* scan = line_start;
        bool valid = true;
        
        while (scan < line_end) {
          const unsigned char ch = *scan;
          if (ch <= 0x7F) {
            ++scan;
            continue;
          }
          
          if (ch >= 0xC2 && ch <= 0xDF) {
            if (scan + 1 >= line_end || (scan[1] & 0xC0U) != 0x80U) {
              valid = false;
              break;
            }
            scan += 2;
            continue;
          }
          
          if (ch == 0xE0) {
            if (scan + 2 >= line_end || scan[1] < 0xA0U || scan[1] > 0xBFU || (scan[2] & 0xC0U) != 0x80U) {
              valid = false;
              break;
            }
            scan += 3;
            continue;
          }
          
          if (ch >= 0xE1 && ch <= 0xEC) {
            if (scan + 2 >= line_end || (scan[1] & 0xC0U) != 0x80U || (scan[2] & 0xC0U) != 0x80U) {
              valid = false;
              break;
            }
            scan += 3;
            continue;
          }
          
          if (ch == 0xED) {
            if (scan + 2 >= line_end || scan[1] < 0x80U || scan[1] > 0x9FU || (scan[2] & 0xC0U) != 0x80U) {
              valid = false;
              break;
            }
            scan += 3;
            continue;
          }
          
          if (ch >= 0xEE && ch <= 0xEF) {
            if (scan + 2 >= line_end || (scan[1] & 0xC0U) != 0x80U || (scan[2] & 0xC0U) != 0x80U) {
              valid = false;
              break;
            }
            scan += 3;
            continue;
          }
          
          if (ch == 0xF0) {
            if (scan + 3 >= line_end || scan[1] < 0x90U || scan[1] > 0xBFU || (scan[2] & 0xC0U) != 0x80U || (scan[3] & 0xC0U) != 0x80U) {
              valid = false;
              break;
            }
            scan += 4;
            continue;
          }
          
          if (ch >= 0xF1 && ch <= 0xF3) {
            if (scan + 3 >= line_end || (scan[1] & 0xC0U) != 0x80U || (scan[2] & 0xC0U) != 0x80U || (scan[3] & 0xC0U) != 0x80U) {
              valid = false;
              break;
            }
            scan += 4;
            continue;
          }
          
          if (ch == 0xF4) {
            if (scan + 3 >= line_end || scan[1] < 0x80U || scan[1] > 0x8FU || (scan[2] & 0xC0U) != 0x80U || (scan[3] & 0xC0U) != 0x80U) {
              valid = false;
              break;
            }
            scan += 4;
            continue;
          }
          
          // Invalid UTF-8 start byte
          valid = false;
          break;
        }
        
        if (valid) count++;
        p++;
        break;
      }
      
      // Check for invalid continuation byte at start of sequence
      if (c >= 0x80U && c < 0xC0U) {
        break;
      }
      
      // Check for invalid start byte (0xC0, 0xC1, 0xF5-0xFF)
      if (c == 0xC0 || c == 0xC1 || c >= 0xF5) {
        break;
      }
      
      // Valid ASCII or valid UTF-8 start byte, continue scanning
      ++p;
    }
    
    // Handle last line (no trailing newline)
    if (p == end && line_start < end) {
      const unsigned char* line_end = p;
      const unsigned char* scan = line_start;
      bool valid = true;
      
      while (scan < line_end) {
        const unsigned char ch = *scan;
        if (ch <= 0x7F) {
          ++scan;
          continue;
        }
        
        if (ch >= 0xC2 && ch <= 0xDF) {
          if (scan + 1 >= line_end || (scan[1] & 0xC0U) != 0x80U) {
            valid = false;
            break;
          }
          scan += 2;
          continue;
        }
        
        if (ch == 0xE0) {
          if (scan + 2 >= line_end || scan[1] < 0xA0U || scan[1] > 0xBFU || (scan[2] & 0xC0U) != 0x80U) {
            valid = false;
            break;
          }
          scan += 3;
          continue;
        }
        
        if (ch >= 0xE1 && ch <= 0xEC) {
          if (scan + 2 >= line_end || (scan[1] & 0xC0U) != 0x80U || (scan[2] & 0xC0U) != 0x80U) {
            valid = false;
            break;
          }
          scan += 3;
          continue;
        }
        
        if (ch == 0xED) {
          if (scan + 2 >= line_end || scan[1] < 0x80U || scan[1] > 0x9FU || (scan[2] & 0xC0U) != 0x80U) {
            valid = false;
            break;
          }
          scan += 3;
          continue;
        }
        
        if (ch >= 0xEE && ch <= 0xEF) {
          if (scan + 2 >= line_end || (scan[1] & 0xC0U) != 0x80U || (scan[2] & 0xC0U) != 0x80U) {
            valid = false;
            break;
          }
          scan += 3;
          continue;
        }
        
        if (ch == 0xF0) {
          if (scan + 3 >= line_end || scan[1] < 0x90U || scan[1] > 0xBFU || (scan[2] & 0xC0U) != 0x80U || (scan[3] & 0xC0U) != 0x80U) {
            valid = false;
            break;
          }
          scan += 4;
          continue;
        }
        
        if (ch >= 0xF1 && ch <= 0xF3) {
          if (scan + 3 >= line_end || (scan[1] & 0xC0U) != 0x80U || (scan[2] & 0xC0U) != 0x80U || (scan[3] & 0xC0U) != 0x80U) {
            valid = false;
            break;
          }
          scan += 4;
          continue;
        }
        
        if (ch == 0xF4) {
          if (scan + 3 >= line_end || scan[1] < 0x80U || scan[1] > 0x8FU || (scan[2] & 0xC0U) != 0x80U || (scan[3] & 0xC0U) != 0x80U) {
            valid = false;
            break;
          }
          scan += 4;
          continue;
        }
        
        valid = false;
        break;
      }
      
      if (valid) count++;
    }
  }
  
  return count;
}