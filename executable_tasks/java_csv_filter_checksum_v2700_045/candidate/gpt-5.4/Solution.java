import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int size = rows.size();

    for (int r = 0; r < size; r++) {
      String row = rows.get(r);
      final int len = row.length();
      int i = 0;

      // Skip field 0
      while (i < len && row.charAt(i) != ',') i++;
      if (i < len) i++;

      // Skip field 1
      while (i < len && row.charAt(i) != ',') i++;
      if (i < len) i++;

      // Parse field 2 as int: b
      int sign = 1;
      if (i < len && row.charAt(i) == '-') {
        sign = -1;
        i++;
      }
      int b = 0;
      while (i < len) {
        char ch = row.charAt(i);
        if (ch == ',') break;
        b = b * 10 + (ch - '0');
        i++;
      }
      b *= sign;
      if (i < len) i++;

      // Parse field 3 as int: c
      sign = 1;
      if (i < len && row.charAt(i) == '-') {
        sign = -1;
        i++;
      }
      int c = 0;
      while (i < len) {
        char ch = row.charAt(i);
        if (ch == ',') break;
        c = c * 10 + (ch - '0');
        i++;
      }
      c *= sign;
      if (i < len) i++;

      // Check field 4 equals "hot"
      boolean hot = false;
      int start = i;
      if (start + 2 < len
          && row.charAt(start) == 'h'
          && row.charAt(start + 1) == 'o'
          && row.charAt(start + 2) == 't') {
        int end = start + 3;
        hot = end == len || row.charAt(end) == ',';
      }

      long v = (b > 300 ? (long) b + c : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }

    return hash;
  }
}
