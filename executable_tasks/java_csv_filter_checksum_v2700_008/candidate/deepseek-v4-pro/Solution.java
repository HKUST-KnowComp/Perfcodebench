import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      int c5 = row.indexOf(',', c4 + 1);

      int bStart = c2 + 1;
      int bEnd = c3;
      int b = 0;
      boolean bNeg = false;
      int i = bStart;
      if (i < bEnd && row.charAt(i) == '-') {
        bNeg = true;
        i++;
      } else if (i < bEnd && row.charAt(i) == '+') {
        i++;
      }
      for (; i < bEnd; i++) {
        b = b * 10 + (row.charAt(i) - '0');
      }
      if (bNeg) b = -b;

      int cStart = c3 + 1;
      int cEnd = c4;
      int c = 0;
      boolean cNeg = false;
      i = cStart;
      if (i < cEnd && row.charAt(i) == '-') {
        cNeg = true;
        i++;
      } else if (i < cEnd && row.charAt(i) == '+') {
        i++;
      }
      for (; i < cEnd; i++) {
        c = c * 10 + (row.charAt(i) - '0');
      }
      if (cNeg) c = -c;

      int hotStart = c4 + 1;
      int hotEnd = (c5 >= 0) ? c5 : len;
      boolean hot = (hotEnd - hotStart == 3) &&
                    row.charAt(hotStart) == 'h' &&
                    row.charAt(hotStart + 1) == 'o' &&
                    row.charAt(hotStart + 2) == 't';

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}