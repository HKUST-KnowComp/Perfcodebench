import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();
      int commaCount = 0;
      int i = 0;
      int b = 0;
      int c = 0;
      int sign = 1;
      boolean hot = false;

      while (i < len) {
        int start = i;
        while (i < len && row.charAt(i) != ',') {
          i++;
        }

        if (commaCount == 2) {
          int j = start;
          sign = 1;
          if (j < i) {
            char ch = row.charAt(j);
            if (ch == '-') {
              sign = -1;
              j++;
            } else if (ch == '+') {
              j++;
            }
          }
          int val = 0;
          while (j < i) {
            val = val * 10 + (row.charAt(j) - '0');
            j++;
          }
          b = val * sign;
        } else if (commaCount == 3) {
          int j = start;
          sign = 1;
          if (j < i) {
            char ch = row.charAt(j);
            if (ch == '-') {
              sign = -1;
              j++;
            } else if (ch == '+') {
              j++;
            }
          }
          int val = 0;
          while (j < i) {
            val = val * 10 + (row.charAt(j) - '0');
            j++;
          }
          c = val * sign;
        } else if (commaCount == 4) {
          int fieldLen = i - start;
          hot = fieldLen == 3
              && row.charAt(start) == 'h'
              && row.charAt(start + 1) == 'o'
              && row.charAt(start + 2) == 't';
          break;
        }

        commaCount++;
        i++; // skip comma
      }

      long v = (b > 300 ? (long) b + c : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= prime;
    }

    return hash;
  }
}
