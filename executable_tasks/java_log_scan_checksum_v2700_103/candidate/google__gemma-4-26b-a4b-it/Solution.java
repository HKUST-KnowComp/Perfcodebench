import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      // Expected format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
      // We use a manual scan to avoid Regex overhead
      
      int idx = 0;
      // Check "src=S"
      if (len < 5 || row.charAt(0) != 's' || row.charAt(1) != 'r' || row.charAt(2) != 'c' || row.charAt(3) != '=' || row.charAt(4) != 'S') continue;
      idx = 5;
      
      // Parse src
      int src = 0;
      while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') {
        src = src * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      if (idx >= len || row.charAt(idx) != ' ') continue;
      idx++; // skip space

      // Check "lat="
      if (idx + 4 > len || row.charAt(idx) != 'l' || row.charAt(idx+1) != 'a' || row.charAt(idx+2) != 't' || row.charAt(idx+3) != '=') continue;
      idx += 4;

      // Parse lat
      int lat = 0;
      while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') {
        lat = lat * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      if (idx >= len || row.charAt(idx) != ' ') continue;
      idx++; // skip space

      // Check "code="
      if (idx + 5 > len || row.charAt(idx) != 'c' || row.charAt(idx+1) != 'o' || row.charAt(idx+2) != 'd' || row.charAt(idx+3) != 'e' || row.charAt(idx+4) != '=') continue;
      idx += 5;

      // Parse code
      int code = 0;
      while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') {
        code = code * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      if (idx >= len || row.charAt(idx) != ' ') continue;
      idx++; // skip space

      // Check "ok="
      if (idx + 3 > len || row.charAt(idx) != 'o' || row.charAt(idx+1) != 'k' || row.charAt(idx+2) != '=') continue;
      idx += 3;

      // Parse ok (yes/no)
      boolean ok = false;
      if (idx + 3 <= len && row.charAt(idx) == 'y' && row.charAt(idx+1) == 'e' && row.charAt(idx+2) == 's') {
        ok = true;
        idx += 3;
      } else if (idx + 2 <= len && row.charAt(idx) == 'n' && row.charAt(idx+1) == 'o') {
        ok = false;
        idx += 2;
      } else {
        continue;
      }

      // Ensure end of string
      if (idx != len) continue;

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}