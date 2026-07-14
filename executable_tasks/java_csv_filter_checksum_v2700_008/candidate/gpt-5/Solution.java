import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long hash = FNV_OFFSET;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int len = s.length();

      // Find start index of field #2 (zero-based): skip first two commas
      int i = s.indexOf(',');
      if (i >= 0) {
        i = s.indexOf(',', i + 1);
        i = (i >= 0) ? (i + 1) : len;
      } else {
        i = len;
      }

      // Parse b (field index 2)
      int b = 0;
      boolean neg = false;
      if (i < len) {
        char ch = s.charAt(i);
        if (ch == '-' || ch == '+') {
          neg = (ch == '-');
          i++;
        }
        while (i < len) {
          ch = s.charAt(i);
          if (ch == ',') {
            i++;
            break;
          }
          b = b * 10 + (ch - '0');
          i++;
        }
        if (neg) b = -b;
      }

      // Parse c (field index 3)
      int c = 0;
      neg = false;
      if (i < len) {
        char ch = s.charAt(i);
        if (ch == '-' || ch == '+') {
          neg = (ch == '-');
          i++;
        }
        while (i < len) {
          ch = s.charAt(i);
          if (ch == ',') {
            i++;
            break;
          }
          c = c * 10 + (ch - '0');
          i++;
        }
        if (neg) c = -c;
      }

      // Check if field 4 equals "hot"
      boolean hot = false;
      if (i + 2 < len && s.charAt(i) == 'h' && s.charAt(i + 1) == 'o' && s.charAt(i + 2) == 't') {
        int end = i + 3;
        if (end == len || s.charAt(end) == ',') {
          hot = true;
        }
      }

      long v = ((b > 300) ? ((long) b + (long) c) : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
