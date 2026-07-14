import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx = 0;
      idx = row.indexOf(',', idx) + 1;
      idx = row.indexOf(',', idx) + 1;
      int bStart = idx;
      idx = row.indexOf(',', idx);
      int bEnd = idx;
      int cStart = idx + 1;
      idx = row.indexOf(',', idx + 1);
      int cEnd = idx;
      int hotStart = idx + 1;

      int b = 0;
      int i = bStart;
      boolean bNeg = false;
      if (i < bEnd && row.charAt(i) == '-') { bNeg = true; i++; }
      for (; i < bEnd; i++) b = b * 10 + (row.charAt(i) - '0');
      if (bNeg) b = -b;

      int c = 0;
      i = cStart;
      boolean cNeg = false;
      if (i < cEnd && row.charAt(i) == '-') { cNeg = true; i++; }
      for (; i < cEnd; i++) c = c * 10 + (row.charAt(i) - '0');
      if (cNeg) c = -c;

      boolean hot = (row.length() - hotStart == 3) &&
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