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
      int fieldStart = 0;
      boolean hot = false;

      while (idx <= len) {
        if (idx == len || row.charAt(idx) == ',') {
          commaCount++;
          if (commaCount == 3) {
            int j = fieldStart;
            int sign = 1;
            if (j < idx) {
              char ch = row.charAt(j);
              if (ch == '-') {
                sign = -1;
                j++;
              } else if (ch == '+') {
                j++;
              }
            }
            int val = 0;
            while (j < idx) {
              val = val * 10 + (row.charAt(j) - '0');
              j++;
            }
            b = val * sign;
          } else if (commaCount == 4) {
            int j = fieldStart;
            int sign = 1;
            if (j < idx) {
              char ch = row.charAt(j);
              if (ch == '-') {
                sign = -1;
                j++;
              } else if (ch == '+') {
                j++;
              }
            }
            int val = 0;
            while (j < idx) {
              val = val * 10 + (row.charAt(j) - '0');
              j++;
            }
            c = val * sign;
          } else if (commaCount == 5) {
            hot = (idx - fieldStart == 3)
                && row.charAt(fieldStart) == 'h'
                && row.charAt(fieldStart + 1) == 'o'
                && row.charAt(fieldStart + 2) == 't';
            break;
          }
          fieldStart = idx + 1;
        }
        idx++;
      }

      long v = (b > 300 ? (long) b + c : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
