import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      int fieldIdx = 0;
      int fieldStart = 0;
      int b = 0, c = 0;
      boolean hot = false;
      for (int j = 0; j < len; j++) {
        if (row.charAt(j) == ',') {
          if (fieldIdx == 2) {
            b = parseNum(row, fieldStart, j);
          } else if (fieldIdx == 3) {
            c = parseNum(row, fieldStart, j);
          } else if (fieldIdx == 4) {
            int hLen = j - fieldStart;
            hot = (hLen == 3) && row.charAt(fieldStart) == 'h' && row.charAt(fieldStart + 1) == 'o' && row.charAt(fieldStart + 2) == 't';
            break;
          }
          fieldIdx++;
          fieldStart = j + 1;
        }
      }
      if (fieldIdx == 4) {
        int hLen = len - fieldStart;
        hot = (hLen == 3) && row.charAt(fieldStart) == 'h' && row.charAt(fieldStart + 1) == 'o' && row.charAt(fieldStart + 2) == 't';
      }

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseNum(String s, int start, int end) {
    int res = 0;
    int i = start;
    boolean neg = false;
    if (i < end && s.charAt(i) == '-') {
      neg = true;
      i++;
    }
    for (; i < end; i++) {
      res = res * 10 + (s.charAt(i) - '0');
    }
    return neg ? -res : res;
  }
}