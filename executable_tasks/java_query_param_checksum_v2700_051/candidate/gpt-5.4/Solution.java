import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (int idx = 0, n = rows.size(); idx < n; idx++) {
      String row = rows.get(idx);
      int len = row.length();
      int i = 2; // skip "u="

      int u = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') {
          i += 6; // skip "&sess="
          break;
        }
        u = u * 10 + (c - '0');
        i++;
      }

      int s = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') {
          i += 5; // skip "&flag="
          break;
        }
        s = s * 10 + (c - '0');
        i++;
      }

      int f = 0;
      while (i < len) {
        f = f * 10 + (row.charAt(i) - '0');
        i++;
      }

      long v = (u & 2047) + (long) s + 19L * f;
      hash ^= (v + 1L);
      hash *= prime;
    }

    return hash;
  }
}
