import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV64_OFFSET;
    final int n = rows.size();
    for (int r = 0; r < n; r++) {
      String row = rows.get(r);
      int len = row.length();
      int i = 2; // skip "u="

      // parse u until '&'
      int u = 0;
      boolean neg = false;
      char ch = row.charAt(i);
      if (ch == '-') { neg = true; i++; }
      for (;; i++) {
        ch = row.charAt(i);
        if (ch == '&') { i++; break; }
        u = u * 10 + (ch - '0');
      }
      if (neg) u = -u;

      // second param: skip "state=" (6 chars)
      i += 6;
      int s = 0;
      neg = false;
      ch = row.charAt(i);
      if (ch == '-') { neg = true; i++; }
      for (;; i++) {
        ch = row.charAt(i);
        if (ch == '&') { i++; break; }
        s = s * 10 + (ch - '0');
      }
      if (neg) s = -s;

      // third param: skip "flag=" (5 chars)
      i += 5;
      int f = 0;
      neg = false;
      if (i < len) {
        ch = row.charAt(i);
        if (ch == '-') { neg = true; i++; }
        for (; i < len; i++) {
          f = f * 10 + (row.charAt(i) - '0');
        }
      }
      if (neg) f = -f;

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= FNV64_PRIME;
    }
    return hash;
  }
}
