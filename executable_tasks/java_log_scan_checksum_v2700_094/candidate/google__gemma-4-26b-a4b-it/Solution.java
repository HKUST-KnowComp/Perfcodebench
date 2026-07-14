import java.util.*;

public class Solution {

  public static long run(List<String> rows) {
    long hash = 146959810394665603L; // Corrected constant from original snippet if it was a typo, but keeping logic identical to baseline
    // Re-evaluating baseline constant: 1469598103934665603L
    hash = 1469598103934665603L;

    for (String row : rows) {
      if (row == null || row.length() < 25) continue; // Basic safety check
      
      char[] chars = row.toCharArray();
      int n = chars.length;

      // Expected format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
      // Manual parsing to avoid Regex overhead
      
      int idx = 0;
      // Check "src=S"
      if (n < 5 || chars[0] != 's' || chars[1] != 'r' || chars[2] != 'c' || chars[3] != '=' || chars[4] != 'S') continue;
      idx = 5;

      // Parse src
      int src = 0;
      while (idx < n && chars[idx] >= '0' && chars[idx] <= '9') {
        src = src * 10 + (chars[idx] - '0');
        idx++;
      }
      if (idx >= n || chars[idx] != ' ') continue;
      idx++; // skip space

      // Check "lat="
      if (idx + 4 > n || chars[idx] != 'l' || chars[idx+1] != 'a' || chars[idx+2] != 't' || chars[idx+3] != '=') continue;
      idx += 4;

      // Parse lat
      int lat = 0;
      while (idx < n && chars[idx] >= '0' && chars[idx] <= '9') {
        lat = lat * 10 + (chars[idx] - '0');
        idx++;
      }
      if (idx >= n || chars[idx] != ' ') continue;
      idx++; // skip space

      // Check "code="
      if (idx + 5 > n || chars[idx] != 'c' || chars[idx+1] != 'o' || chars[idx+2] != 'd' || chars[idx+3] != 'e' || chars[idx+4] != '=') continue;
      idx += 5;

      // Parse code
      int code = 0;
      while (idx < n && chars[idx] >= '0' && chars[idx] <= '9') {
        code = code * 10 + (chars[idx] - '0');
        idx++;
      }
      if (idx >= n || chars[idx] != ' ') continue;
      idx++; // skip space

      // Check "ok="
      if (idx + 3 > n || chars[idx] != 'o' || chars[idx+1] != 'k' || chars[idx+2] != '=') continue;
      idx += 3;

      // Parse ok (yes/no)
      boolean ok = false;
      if (idx + 3 <= n && chars[idx] == 'y' && chars[idx+1] == 'e' && chars[idx+2] == 's') {
        ok = true;
        idx += 3;
      } else if (idx + 2 <= n && chars[idx] == 'n' && chars[idx+1] == 'o') {
        ok = false;
        idx += 2;
      } else {
        continue;
      }

      // Ensure end of string
      if (idx != n) continue;

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
