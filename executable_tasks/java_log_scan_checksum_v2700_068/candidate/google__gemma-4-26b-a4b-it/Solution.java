import java.util.*;

public class Solution {

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      char[] chars = row.toCharArray();
      int n = chars.length;
      
      // Expected format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
      // We scan manually to avoid Regex overhead
      
      int i = 0;
      // Skip 'src=S'
      if (n < 6 || chars[0] != 's' || chars[1] != 'r' || chars[2] != 'c' || chars[3] != '=' || chars[4] != 'S') continue;
      i = 5;
      
      // Parse src
      int src = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        src = src * 10 + (chars[i] - '0');
        i++;
      }
      
      // Skip ' lat='
      if (i + 6 >= n || chars[i] != ' ' || chars[i+1] != 'l' || chars[i+2] != 'a' || chars[i+3] != 't' || chars[i+4] != '=') continue;
      i += 6;
      
      // Parse lat
      int lat = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        lat = lat * 10 + (chars[i] - '0');
        i++;
      }
      
      // Skip ' code='
      if (i + 7 >= n || chars[i] != ' ' || chars[i+1] != 'c' || chars[i+2] != 'o' || chars[i+3] != 'd' || chars[i+4] != 'e' || chars[i+5] != '=') continue;
      i += 7;
      
      // Parse code
      int code = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        code = code * 10 + (chars[i] - '0');
        i++;
      }
      
      // Skip ' ok='
      if (i + 5 >= n || chars[i] != ' ' || chars[i+1] != 'o' || chars[i+2] != 'k' || chars[i+3] != '=') continue;
      i += 4;
      
      // Parse ok (yes/no)
      boolean ok = false;
      if (i < n && chars[i] == 'y') {
        if (i + 2 < n && chars[i+1] == 'e' && chars[i+2] == 's') {
          ok = true;
          i += 3;
        }
      } else if (i < n && chars[i] == 'n') {
        if (i + 1 < n && chars[i+1] == 'o') {
          ok = false;
          i += 2;
        }
      }
      
      // Check if we reached end of string
      if (i != n) continue;

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}