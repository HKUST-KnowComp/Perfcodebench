import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long hash = FNV_OFFSET;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();

      // Parse u from after "u=" until '&'
      int i = 2; // after "u="
      int sign = 1;
      if (i < len) {
        char ch = row.charAt(i);
        if (ch == '-') { sign = -1; i++; }
        else if (ch == '+') { i++; }
      }
      int val = 0;
      while (i < len) {
        char ch = row.charAt(i);
        if (ch == '&') { i++; break; }
        val = val * 10 + (ch - '0');
        i++;
      }
      int u = val * sign;

      // Parse s from after "score=" until '&'
      i += 6; // skip "score="
      sign = 1;
      if (i < len) {
        char ch = row.charAt(i);
        if (ch == '-') { sign = -1; i++; }
        else if (ch == '+') { i++; }
      }
      val = 0;
      while (i < len) {
        char ch = row.charAt(i);
        if (ch == '&') { i++; break; }
        val = val * 10 + (ch - '0');
        i++;
      }
      int s = val * sign;

      // Parse f from after "flag=" (length 5) until end or '&'
      i += 5; // skip "flag="
      sign = 1;
      if (i < len) {
        char ch = row.charAt(i);
        if (ch == '-') { sign = -1; i++; }
        else if (ch == '+') { i++; }
      }
      val = 0;
      while (i < len) {
        char ch = row.charAt(i);
        if (ch == '&') { break; }
        val = val * 10 + (ch - '0');
        i++;
      }
      int f = val * sign;

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
