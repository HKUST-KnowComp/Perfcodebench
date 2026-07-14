import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long hash = FNV_OFFSET;

    for (int r = 0, rn = rows.size(); r < rn; r++) {
      String row = rows.get(r);
      int n = row.length();
      int i = 0;

      // First part: starts with 2-char key (e.g., "u=")
      i = 2; // skip key
      int sign = 1;
      if (i < n) {
        char ch = row.charAt(i);
        if (ch == '-') { sign = -1; i++; }
        else if (ch == '+') { i++; }
      }
      int u = 0;
      char c;
      while (i < n && (c = row.charAt(i)) != '&') {
        u = u * 10 + (c - '0');
        i++;
      }
      u *= sign;

      // Move to second part start (skip '&') and skip 6-char key
      if (i < n && row.charAt(i) == '&') i++;
      i += 6; // skip key of second part

      int signS = 1;
      if (i < n) {
        char ch = row.charAt(i);
        if (ch == '-') { signS = -1; i++; }
        else if (ch == '+') { i++; }
      }
      int s = 0;
      while (i < n && (c = row.charAt(i)) != '&') {
        s = s * 10 + (c - '0');
        i++;
      }
      s *= signS;

      // Move to third part start (skip '&') and skip 5-char key
      if (i < n && row.charAt(i) == '&') i++;
      i += 5; // skip key of third part

      int signF = 1;
      if (i < n) {
        char ch = row.charAt(i);
        if (ch == '-') { signF = -1; i++; }
        else if (ch == '+') { i++; }
      }
      int f = 0;
      while (i < n && (c = row.charAt(i)) != '&') {
        f = f * 10 + (c - '0');
        i++;
      }
      f *= signF;

      long v = (u & 2047) + (long) s + (long) f * 19L;
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}