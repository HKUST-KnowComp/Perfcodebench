import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Locate space separators with fast native indexOf
      int firstSpace = row.indexOf(' ');
      int secondSpace = row.indexOf(' ', firstSpace + 1);
      int thirdSpace = row.indexOf(' ', secondSpace + 1);
      
      // Parse src (follows "src=S" at index 5)
      int src = 0;
      for (int i = 5; i < firstSpace; i++) {
        src = src * 10 + (row.charAt(i) - '0');
      }
      // Parse lat (follows " lat=" starting at firstSpace+5)
      int lat = 0;
      for (int i = firstSpace + 5; i < secondSpace; i++) {
        lat = lat * 10 + (row.charAt(i) - '0');
      }
      // Parse code (follows " code=" starting at secondSpace+6)
      int code = 0;
      for (int i = secondSpace + 6; i < thirdSpace; i++) {
        code = code * 10 + (row.charAt(i) - '0');
      }
      // Check ok flag directly from character position
      boolean ok = row.charAt(thirdSpace + 4) == 'y';
      
      // Exact same checksum calculation as baseline to preserve correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}