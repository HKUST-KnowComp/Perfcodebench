import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    int n = rows.size();
    for (int ri = 0; ri < n; ri++) {
      String row = rows.get(ri);
      int len = row.length();

      // Parse first value (after first '=') until first '&'
      int eq = row.indexOf('=', 0);
      int amp = row.indexOf('&', eq + 1);
      int end = (amp == -1) ? len : amp;
      int u = 0;
      for (int i = eq + 1; i < end; i++) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        u = u * 10 + (c - '0');
      }

      // Parse second value
      int start = (amp == -1) ? len : amp + 1;
      eq = row.indexOf('=', start);
      amp = row.indexOf('&', eq + 1);
      end = (amp == -1) ? len : amp;
      int s = 0;
      for (int i = eq + 1; i < end; i++) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        s = s * 10 + (c - '0');
      }

      // Parse third value
      start = (amp == -1) ? len : amp + 1;
      eq = row.indexOf('=', start);
      amp = row.indexOf('&', eq + 1);
      end = (amp == -1) ? len : amp;
      int f = 0;
      for (int i = eq + 1; i < end; i++) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        f = f * 10 + (c - '0');
      }

      long v = (u & 2047) + s + (long) f * 19L;
      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
