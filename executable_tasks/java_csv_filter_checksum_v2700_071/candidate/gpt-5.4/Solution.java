import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();
      int field = 0;
      int i = 0;

      int b = 0;
      int c = 0;
      boolean hot = false;

      while (i <= len) {
        int start = i;
        while (i < len && row.charAt(i) != ',') {
          i++;
        }

        if (field == 2 || field == 3) {
          int val = 0;
          int sign = 1;
          int p = start;
          if (p < i) {
            char ch = row.charAt(p);
            if (ch == '-') {
              sign = -1;
              p++;
            } else if (ch == '+') {
              p++;
            }
          }
          while (p < i) {
            val = val * 10 + (row.charAt(p) - '0');
            p++;
          }
          val *= sign;
          if (field == 2) {
            b = val;
          } else {
            c = val;
          }
        } else if (field == 4) {
          hot = (i - start == 3
              && row.charAt(start) == 'h'
              && row.charAt(start + 1) == 'o'
              && row.charAt(start + 2) == 't');
          break;
        }

        field++;
        i++; // skip comma or advance past end sentinel
      }

      long v = (b > 300 ? (long) b + c : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= prime;
    }

    return hash;
  }
}
