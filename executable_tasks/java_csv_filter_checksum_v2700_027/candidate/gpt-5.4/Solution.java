import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int size = rows.size();

    for (int i = 0; i < size; i++) {
      String row = rows.get(i);
      int len = row.length();

      int field = 0;
      int b = 0;
      int c = 0;
      boolean hot = false;

      int num = 0;
      boolean neg = false;

      for (int j = 0; j <= len; j++) {
        char ch = (j == len) ? ',' : row.charAt(j);
        if (ch == ',') {
          if (field == 2) {
            b = neg ? -num : num;
          } else if (field == 3) {
            c = neg ? -num : num;
          } else if (field == 4) {
            hot = (j >= 3)
                && row.charAt(j - 3) == 'h'
                && row.charAt(j - 2) == 'o'
                && row.charAt(j - 1) == 't';
            break;
          }
          field++;
          num = 0;
          neg = false;
        } else if (field == 2 || field == 3) {
          if (ch == '-') {
            neg = true;
          } else {
            num = num * 10 + (ch - '0');
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
