import java.util.*;

public class Solution {

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      if (row == null || row.length() < 25) continue; // Basic safety check
      
      char[] chars = row.toCharArray();
      int n = chars.length;

      // Expected format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
      // We scan manually to avoid Regex overhead
      
      int i = 0;
      // Check "src=S"
      if (chars[i++] != 's' || chars[i++] != 'r' || chars[i++] != 'c' || 
          chars[i++] != '=' || chars[i++] != 'S') continue;

      // Parse src
      int src = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        src = src * 10 + (chars[i++] - '0');
      }
      if (i >= n || chars[i++] != ' ') continue;

      // Check "lat="
      if (chars[i++] != 'l' || chars[i++] != 'a' || chars[i++] != 't' || chars[i++] != '=') continue;

      // Parse lat
      int lat = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        lat = lat * 10 + (chars[i++] - '0');
      }
      if (i >= n || chars[i++] != ' ') continue;

      // Check "code="
      if (chars[i++] != 'c' || chars[i++] != 'o' || chars[i++] != 'd' || chars[i++] != 'e' || chars[i++] != '=') continue;

      // Parse code
      int code = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        code = code * 10 + (chars[i++] - '0');
      }
      if (i >= n || chars[i++] != ' ') continue;

      // Check "ok="
      if (chars[i++] != 'o' || chars[i++] != 'k' || chars[i++] != '=') continue;

      // Parse ok (yes/no)
      boolean ok = false;
      if (chars[i] == 'y') {
        if (i + 2 < n && chars[i+1] == 'e' && chars[i+2] == 's') {
          ok = true;
          i += 3;
        } else {
          continue;
        }
      } else if (chars[i] == 'n') {
        if (i + 1 < n && chars[i+1] == 'o') {
          ok = false;
          i += 2;
        } else {
          continue;
        }
      } else {
        continue;
      }

      // Ensure end of string
      if (i != n) continue;

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}