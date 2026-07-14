import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);

      int len = row.length();
      int p = 2; // after "u="
      int u = 0;
      while (p < len) {
        char c = row.charAt(p);
        if (c == '&') break;
        u = u * 10 + (c - '0');
        p++;
      }

      p += 6; // skip "&seed="
      int s = 0;
      while (p < len) {
        char c = row.charAt(p);
        if (c == '&') break;
        s = s * 10 + (c - '0');
        p++;
      }

      p += 5; // skip "&flag="
      int f = 0;
      while (p < len) {
        f = f * 10 + (row.charAt(p) - '0');
        p++;
      }

      long v = (u & 2047) + (long) s + 19L * f;
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
