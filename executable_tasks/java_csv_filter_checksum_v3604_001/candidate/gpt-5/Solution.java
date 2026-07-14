import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();
      int field = 0;
      int start = 0;
      int b = 0, c = 0;
      boolean hot = false;
      boolean gotB = false, gotC = false, gotHot = false;

      // Scan the row once, process only fields 2, 3, and 4. Break early when done.
      for (int i = 0;; i++) {
        boolean end = i >= len;
        if (end || row.charAt(i) == ',') {
          if (field == 2) {
            b = fastParseInt(row, start, i);
            gotB = true;
          } else if (field == 3) {
            c = fastParseInt(row, start, i);
            gotC = true;
          } else if (field == 4) {
            int l = i - start;
            if (l == 3 && row.charAt(start) == 'h' && row.charAt(start + 1) == 'o' && row.charAt(start + 2) == 't') {
              hot = true;
            } else {
              hot = false;
            }
            gotHot = true;
          }
          field++;
          if (gotB && gotC && gotHot) {
            break;
          }
          if (end) {
            break;
          }
          start = i + 1;
        }
      }

      int acc = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      long v = (long) acc;
      hash ^= (v + 1L);
      hash *= prime;
    }

    return hash;
  }

  private static int fastParseInt(String s, int start, int end) {
    int i = start;
    boolean neg = false;
    if (i < end) {
      char ch = s.charAt(i);
      if (ch == '-') {
        neg = true;
        i++;
      } else if (ch == '+') {
        i++;
      }
    }
    int val = 0;
    for (; i < end; i++) {
      int d = s.charAt(i) - '0';
      val = val * 10 + d;
    }
    return neg ? -val : val;
  }
}