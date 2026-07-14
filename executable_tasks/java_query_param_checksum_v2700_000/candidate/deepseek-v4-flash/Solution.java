import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx = 2; // skip "u="
      int u = 0;
      while (row.charAt(idx) != '&') {
        u = u * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      // skip '&'
      idx++;
      // skip "status=" (7 chars)
      idx += 7;
      int s = 0;
      while (row.charAt(idx) != '&') {
        s = s * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      // skip '&'
      idx++;
      // skip "flag=" (5 chars)
      idx += 5;
      int f = 0;
      while (idx < row.length()) {
        f = f * 10 + (row.charAt(idx) - '0');
        idx++;
      }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}