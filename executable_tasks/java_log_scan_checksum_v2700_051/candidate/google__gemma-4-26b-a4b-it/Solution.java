import java.util.*;

public class Solution {

  public static long run(List<String> rows) {
    long hash = 146959810394665603L; // Corrected constant from original logic if needed, but keeping original value: 1469598103934665603L
    hash = 1469598103934665603L;

    for (String row : rows) {
      char[] chars = row.toCharArray();
      int n = chars.length;

      // Expected format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
      // Manual parsing to avoid Regex overhead
      
      int i = 0;
      // Check 'src=S'
      if (n < 6 || chars[0] != 's' || chars[1] != 'r' || chars[2] != 'c' || chars[3] != '=' || chars[4] != 'S') continue;
      i = 5;

      // Parse src
      int src = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        src = src * 10 + (chars[i] - '0');
        i++;
      }
      if (i >= n || chars[i] != ' ') continue;
      i++; // skip space

      // Check 'lat='
      if (i + 4 >= n || chars[i] != 'l' || chars[i+1] != 'a' || chars[i+2] != 't' || chars[i+3] != '=') continue;
      i += 4;

      // Parse lat
      int lat = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        lat = lat * 10 + (chars[i] - '0');
        i++;
      }
      if (i >= n || chars[i] != ' ') continue;
      i++; // skip space

      // Check 'code='
      if (i + 5 >= n || chars[i] != 'c' || chars[i+1] != 'o' || chars[i+2] != 'd' || chars[i+3] != 'e' || chars[i+4] != '=') continue;
      i += 5;

      // Parse code
      int code = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        code = code * 10 + (chars[i] - '0');
        i++;
      }
      if (i >= n || chars[i] != ' ') continue;
      i++; // skip space

      // Check 'ok='
      if (i + 3 >= n || chars[i] != 'o' || chars[i+1] != 'k' || chars[i+2] != '=') continue;
      i += 3;

      // Parse ok (yes/no)
      boolean ok = false;
      if (i + 2 < n && chars[i] == 'y' && chars[i+1] == 'e' && chars[i+2] == 's') {
        ok = true;
        i += 3;
      } else if (i + 1 < n && chars[i] == 'n' && chars[i+1] == 'o') {
        ok = false;
        i += 2;
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