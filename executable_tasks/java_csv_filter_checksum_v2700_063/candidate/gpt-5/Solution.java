import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long hash = FNV_OFFSET;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();
      int idx = 0;

      // Skip first two fields
      for (int skip = 0; skip < 2; skip++) {
        while (idx < len && row.charAt(idx) != ',') {
          idx++;
        }
        // move past comma if present
        if (idx < len) idx++;
      }

      // Parse field 3 (b)
      int b = 0;
      boolean neg = false;
      if (idx < len) {
        char ch = row.charAt(idx);
        if (ch == '-') { neg = true; idx++; }
        else if (ch == '+') { idx++; }
      }
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') { idx++; break; }
        b = b * 10 + (ch - '0');
        idx++;
      }
      if (neg) b = -b;

      // Parse field 4 (c)
      int c = 0;
      neg = false;
      if (idx < len) {
        char ch = row.charAt(idx);
        if (ch == '-') { neg = true; idx++; }
        else if (ch == '+') { idx++; }
      }
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') { idx++; break; }
        c = c * 10 + (ch - '0');
        idx++;
      }
      if (neg) c = -c;

      // Parse field 5 and check for "hot"
      int start = idx;
      int end = start;
      while (end < len && row.charAt(end) != ',') {
        end++;
      }
      boolean hot = false;
      if (end - start == 3) {
        if (row.charAt(start) == 'h' && row.charAt(start + 1) == 'o' && row.charAt(start + 2) == 't') {
          hot = true;
        }
      }

      long v = (b > 300 ? ((long) b + (long) c) : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}