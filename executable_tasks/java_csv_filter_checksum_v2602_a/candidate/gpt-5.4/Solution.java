import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int size = rows.size();

    for (int i = 0; i < size; i++) {
      String row = rows.get(i);
      int len = row.length();

      int commaCount = 0;
      int idx = 0;
      int b = 0;
      int c = 0;
      boolean hot = false;

      while (idx < len) {
        int start = idx;
        while (idx < len && row.charAt(idx) != ',') {
          idx++;
        }

        if (commaCount == 2) {
          int p = start;
          boolean neg = false;
          if (p < idx && row.charAt(p) == '-') {
            neg = true;
            p++;
          }
          int val = 0;
          while (p < idx) {
            val = val * 10 + (row.charAt(p) - '0');
            p++;
          }
          b = neg ? -val : val;
        } else if (commaCount == 3) {
          int p = start;
          boolean neg = false;
          if (p < idx && row.charAt(p) == '-') {
            neg = true;
            p++;
          }
          int val = 0;
          while (p < idx) {
            val = val * 10 + (row.charAt(p) - '0');
            p++;
          }
          c = neg ? -val : val;
        } else if (commaCount == 4) {
          int fieldLen = idx - start;
          hot = fieldLen == 3
              && row.charAt(start) == 'h'
              && row.charAt(start + 1) == 'o'
              && row.charAt(start + 2) == 't';
          break;
        }

        commaCount++;
        idx++;
      }

      if (commaCount == 4 && idx >= len) {
        int start = 0;
        int seen = 0;
        for (int j = 0; j < len; j++) {
          if (row.charAt(j) == ',') {
            seen++;
            start = j + 1;
            if (seen == 4) {
              int fieldLen = len - start;
              hot = fieldLen == 3
                  && row.charAt(start) == 'h'
                  && row.charAt(start + 1) == 'o'
                  && row.charAt(start + 2) == 't';
              break;
            }
          }
        }
      }

      long v = (b > 300 ? (long) b + c : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }

    return hash;
  }
}
