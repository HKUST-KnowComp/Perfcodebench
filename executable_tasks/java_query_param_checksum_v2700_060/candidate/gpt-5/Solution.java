import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV_OFFSET;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();

      // Parse u starting after "u=" (2 chars)
      int i = 2;
      int sign = 1;
      int u = 0;
      if (i < len) {
        char ch = row.charAt(i);
        if (ch == '-') { sign = -1; i++; }
        while (i < len) {
          ch = row.charAt(i);
          if (ch == '&') break;
          u = u * 10 + (ch - '0');
          i++;
        }
        u *= sign;
      }

      // Skip '&' and "score=" (1 + 6)
      i += 7;

      // Parse s
      sign = 1;
      int s = 0;
      if (i < len) {
        char ch = row.charAt(i);
        if (ch == '-') { sign = -1; i++; }
        while (i < len) {
          ch = row.charAt(i);
          if (ch == '&') break;
          s = s * 10 + (ch - '0');
          i++;
        }
        s *= sign;
      }

      // Skip '&' and "flag=" (1 + 5)
      i += 6;

      // Parse f
      sign = 1;
      int f = 0;
      if (i < len) {
        char ch = row.charAt(i);
        if (ch == '-') { sign = -1; i++; }
        while (i < len) {
          ch = row.charAt(i);
          if (ch == '&') break; // in case there are extra params
          f = f * 10 + (ch - '0');
          i++;
        }
        f *= sign;
      }

      long v = (u & 2047) + (long) s + (long) f * 19L;
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }
    return hash;
  }
}