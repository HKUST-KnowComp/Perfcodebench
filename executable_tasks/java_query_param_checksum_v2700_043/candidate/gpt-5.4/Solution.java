import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      int idx = 2; // after "u="

      int u = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c == '&') {
          idx += 6; // skip "&sess="
          break;
        }
        u = u * 10 + (c - '0');
        idx++;
      }

      int s = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c == '&') {
          idx += 5; // skip "&flag="
          break;
        }
        s = s * 10 + (c - '0');
        idx++;
      }

      int f = 0;
      while (idx < len) {
        f = f * 10 + (row.charAt(idx) - '0');
        idx++;
      }

      long v = (u & 2047) + (long) s + 19L * f;
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
