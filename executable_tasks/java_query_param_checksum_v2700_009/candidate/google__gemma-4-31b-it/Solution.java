import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int pos = 0;

      // Field 1: u (starts at index 2)
      pos += 2;
      int u = 0;
      while (pos < len && row.charAt(pos) != '&') {
        u = u * 10 + (row.charAt(pos++) - '0');
      }
      pos++; // skip '&'

      // Field 2: s (starts at index 6 relative to start of field)
      pos += 6;
      int s = 0;
      while (pos < len && row.charAt(pos) != '&') {
        s = s * 10 + (row.charAt(pos++) - '0');
      }
      pos++; // skip '&'

      // Field 3: f (starts at index 5 relative to start of field)
      pos += 5;
      int f = 0;
      while (pos < len) {
        f = f * 10 + (row.charAt(pos++) - '0');
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}