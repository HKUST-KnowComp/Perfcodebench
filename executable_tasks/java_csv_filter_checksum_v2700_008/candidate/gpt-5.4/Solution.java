import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();

      int commaCount = 0;
      int idx = 0;
      while (idx < len && commaCount < 2) {
        if (row.charAt(idx++) == ',') commaCount++;
      }

      int b = 0;
      while (idx < len) {
        char ch = row.charAt(idx++);
        if (ch == ',') break;
        b = b * 10 + (ch - '0');
      }

      int c = 0;
      while (idx < len) {
        char ch = row.charAt(idx++);
        if (ch == ',') break;
        c = c * 10 + (ch - '0');
      }

      boolean hot = false;
      if (idx + 2 < len) {
        hot = row.charAt(idx) == 'h'
            && row.charAt(idx + 1) == 'o'
            && row.charAt(idx + 2) == 't'
            && (idx + 3 == len || row.charAt(idx + 3) == ',');
      }

      long v = (b > 300 ? (long) b + c : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
