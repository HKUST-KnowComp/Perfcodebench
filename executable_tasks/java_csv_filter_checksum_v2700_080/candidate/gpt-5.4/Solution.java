import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int size = rows.size();

    for (int i = 0; i < size; i++) {
      String row = rows.get(i);
      int len = row.length();

      int field = 0;
      int idx = 0;
      int b = 0;
      int c = 0;
      boolean hot = false;

      while (idx < len && field < 5) {
        int start = idx;
        while (idx < len && row.charAt(idx) != ',') {
          idx++;
        }

        if (field == 2 || field == 3) {
          int val = 0;
          int p = start;
          boolean neg = false;
          if (p < idx && row.charAt(p) == '-') {
            neg = true;
            p++;
          }
          while (p < idx) {
            val = val * 10 + (row.charAt(p) - '0');
            p++;
          }
          if (neg) val = -val;
          if (field == 2) {
            b = val;
          } else {
            c = val;
          }
        } else if (field == 4) {
          hot = (idx - start == 3
              && row.charAt(start) == 'h'
              && row.charAt(start + 1) == 'o'
              && row.charAt(start + 2) == 't');
          break;
        }

        idx++; // skip comma
        field++;
      }

      long v = (b > 300 ? (long) b + c : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }

    return hash;
  }
}
