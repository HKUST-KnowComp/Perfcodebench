import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int size = rows.size();
    for (int r = 0; r < size; r++) {
      String row = rows.get(r);
      int len = row.length();
      int i = 2; // skip first key (length 2, e.g., "u=")

      // Parse first int (u) until '&'
      int u = 0;
      int sign = 1;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-') { sign = -1; i++; }
        else if (c == '+') { i++; }
      }
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') { i++; break; }
        u = u * 10 + (c - '0');
        i++;
      }
      u *= sign;

      // Second part: skip key of length 6 (e.g., "state=")
      i += 6;
      int s = 0;
      sign = 1;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-') { sign = -1; i++; }
        else if (c == '+') { i++; }
      }
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') { i++; break; }
        s = s * 10 + (c - '0');
        i++;
      }
      s *= sign;

      // Third part: skip key of length 5 (e.g., "flag=")
      i += 5;
      int f = 0;
      sign = 1;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-') { sign = -1; i++; }
        else if (c == '+') { i++; }
      }
      while (i < len) {
        char c = row.charAt(i);
        // third value runs to end of string
        f = f * 10 + (c - '0');
        i++;
      }
      f *= sign;

      long v = (u & 2047) + (long) s + 19L * (long) f;
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
