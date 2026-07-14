import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_MULT = 1099511628211L;
    for (String row : rows) {
      int len = row.length();
      int ptr = 5; // Skip leading "src=S"
      
      // Parse src value
      int src = 0;
      while (ptr < len) {
        char c = row.charAt(ptr);
        if (c == ' ') break;
        src = src * 10 + (c - '0');
        ptr++;
      }
      
      // Skip " lat=" to start of lat value
      ptr += 5;
      int lat = 0;
      while (ptr < len) {
        char c = row.charAt(ptr);
        if (c == ' ') break;
        lat = lat * 10 + (c - '0');
        ptr++;
      }
      
      // Skip " code=" to start of code value
      ptr += 6;
      int code = 0;
      while (ptr < len) {
        char c = row.charAt(ptr);
        if (c == ' ') break;
        code = code * 10 + (c - '0');
        ptr++;
      }
      
      // Skip " ok=" to check ok flag
      ptr += 4;
      boolean ok = row.charAt(ptr) == 'y';
      
      // Original hash calculation preserved for correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= FNV_MULT;
    }
    return hash;
  }
}