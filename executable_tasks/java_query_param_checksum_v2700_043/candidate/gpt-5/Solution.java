import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();

      int a = -1; // index of first '&'
      int b = -1; // index of second '&'
      for (int i = 0; i < len; i++) {
        if (row.charAt(i) == '&') {
          if (a == -1) {
            a = i;
          } else {
            b = i;
            break;
          }
        }
      }
      // Fallbacks (should not trigger with valid input, but keep safe)
      if (a == -1) a = row.indexOf('&');
      if (b == -1 && a != -1) b = row.indexOf('&', a + 1);
      if (a == -1 || b == -1) {
        // Input format unexpected; mimic baseline behavior would throw earlier.
        // Here, we avoid crashes by treating missing parts as zeros.
        // However, per task spec inputs are well-formed.
        continue;
      }

      int u = parseInt(row, 2, a);           // parts[0].substring(2)
      int s = parseInt(row, a + 7, b);       // parts[1].substring(6)
      int f = parseInt(row, b + 6, len);     // parts[2].substring(5)

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int val = 0;
    for (int i = start; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return val;
  }
}
