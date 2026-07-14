import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int rIndex = 0, size = rows.size(); rIndex < size; rIndex++) {
      String row = rows.get(rIndex);
      int len = row.length();
      int i = 2; // skip "u="

      // Parse u until '&'
      boolean neg = false;
      int u = 0;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-' || c == '+') {
          neg = (c == '-');
          i++;
        }
      }
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') {
          i++;
          break;
        }
        u = u * 10 + (c - '0');
        i++;
      }
      if (neg) u = -u;

      // Skip "score=" (length 6)
      i += 6;

      // Parse s until '&'
      neg = false;
      int s = 0;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-' || c == '+') {
          neg = (c == '-');
          i++;
        }
      }
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') {
          i++;
          break;
        }
        s = s * 10 + (c - '0');
        i++;
      }
      if (neg) s = -s;

      // Skip "flag=" (length 5)
      i += 5;

      // Parse f until end (or unexpected '&')
      neg = false;
      int f = 0;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-' || c == '+') {
          neg = (c == '-');
          i++;
        }
      }
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') {
          break;
        }
        f = f * 10 + (c - '0');
        i++;
      }
      if (neg) f = -f;

      long v = (u & 2047) + (long) s + (long) f * 19L;
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}