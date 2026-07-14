import java.util.*;

public class Solution {
  private static final long OFFSET = 1469598103934665603L;
  private static final long PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = OFFSET;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();

      // Parse first param: starts with key of length 2 (e.g., "u=")
      int i = 2;
      int u = 0;
      boolean neg = false;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-') { neg = true; i++; }
        else if (c == '+') { i++; }
        while (i < len) {
          c = row.charAt(i);
          if (c == '&') { i++; break; }
          u = u * 10 + (c - '0');
          i++;
        }
      }
      if (neg) u = -u;

      // Parse second param: skip key of length 6 (e.g., "score=")
      i += 6;
      int s = 0;
      neg = false;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-') { neg = true; i++; }
        else if (c == '+') { i++; }
        while (i < len) {
          c = row.charAt(i);
          if (c == '&') { i++; break; }
          s = s * 10 + (c - '0');
          i++;
        }
      }
      if (neg) s = -s;

      // Parse third param: skip key of length 5 (e.g., "flag=")
      i += 5;
      int f = 0;
      neg = false;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-') { neg = true; i++; }
        else if (c == '+') { i++; }
        while (i < len) {
          c = row.charAt(i);
          if (c == '&') { break; }
          f = f * 10 + (c - '0');
          i++;
        }
      }
      if (neg) f = -f;

      long v = ((long)(u & 2047)) + (long)s + ((long)f) * 19L;
      hash ^= (v + 1L);
      hash *= PRIME;
    }
    return hash;
  }
}