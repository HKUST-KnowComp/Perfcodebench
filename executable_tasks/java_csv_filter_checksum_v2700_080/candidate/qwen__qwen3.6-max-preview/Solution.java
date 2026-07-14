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
      if (c5 == -1) c5 = len;

      int b = 0;
      int bStart = c2 + 1;
      boolean bNeg = false;
      if (bStart < c3) {
        char fc = row.charAt(bStart);
        if (fc == '-') { bNeg = true; bStart++; }
        else if (fc == '+') { bStart++; }
      }
      for (int j = bStart; j < c3; j++) {
        b = b * 10 + (row.charAt(j) - '0');
      }
      if (bNeg) b = -b;

      int cVal = 0;
      int cStart = c3 + 1;
      boolean cNeg = false;
      if (cStart < c4) {
        char fc = row.charAt(cStart);
        if (fc == '-') { cNeg = true; cStart++; }
        else if (fc == '+') { cStart++; }
      }
      for (int j = cStart; j < c4; j++) {
        cVal = cVal * 10 + (row.charAt(j) - '0');
      }
      if (cNeg) cVal = -cVal;

      int hStart = c4 + 1;
      boolean hot = (c5 - hStart == 3) &&
                    row.charAt(hStart) == 'h' &&
                    row.charAt(hStart + 1) == 'o' &&
                    row.charAt(hStart + 2) == 't';

      long v = (b > 300 ? (b + cVal) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}