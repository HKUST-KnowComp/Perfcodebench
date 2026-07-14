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

      // Find start of field 2 (0-based), i.e. after second comma
      while (idx < len && commaCount < 2) {
        if (row.charAt(idx++) == ',') commaCount++;
      }

      // Parse integer field 2: b
      int b = 0;
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') {
          idx++;
          break;
        }
        b = b * 10 + (ch - '0');
        idx++;
      }

      // Parse integer field 3: c
      int c = 0;
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') {
          idx++;
          break;
        }
        c = c * 10 + (ch - '0');
        idx++;
      }

      // Check whether field 4 equals "hot"
      boolean hot = false;
      if (idx + 2 < len
          && row.charAt(idx) == 'h'
          && row.charAt(idx + 1) == 'o'
          && row.charAt(idx + 2) == 't'
          && (idx + 3 == len || row.charAt(idx + 3) == ',')) {
        hot = true;
      }

      long v = (b > 300 ? (long) b + c : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }

    return hash;
  }
}
