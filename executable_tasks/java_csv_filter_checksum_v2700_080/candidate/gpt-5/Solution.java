import java.util.*;

public class Solution {
  private static int parseIntFast(String s, int start, int end) {
    int i = start;
    int sign = 1;
    if (i < end) {
      char ch = s.charAt(i);
      if (ch == '-') {
        sign = -1;
        i++;
      } else if (ch == '+') {
        i++;
      }
    }
    int val = 0;
    for (; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return sign * val;
  }

  public static long run(List<String> rows) {
    final long FNV64_OFFSET_BASIS = 1469598103934665603L;
    final long FNV64_PRIME = 1099511628211L;
    long hash = FNV64_OFFSET_BASIS;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();

      // Find comma positions to delimit fields quickly
      int p0 = row.indexOf(',', 0);          // end of field 0
      int p1 = row.indexOf(',', p0 + 1);     // end of field 1
      int p2 = row.indexOf(',', p1 + 1);     // end of field 2 (b)
      int p3 = row.indexOf(',', p2 + 1);     // end of field 3 (c)

      // Parse b (field 2)
      int b = parseIntFast(row, p1 + 1, p2);
      // Parse c (field 3)
      int c = parseIntFast(row, p2 + 1, p3);

      // Field 4 (check for "hot")
      int f4Start = p3 + 1;
      int f4End = row.indexOf(',', f4Start);
      if (f4End < 0) f4End = len;
      boolean hot = (f4End - f4Start == 3)
          && row.charAt(f4Start) == 'h'
          && row.charAt(f4Start + 1) == 'o'
          && row.charAt(f4Start + 2) == 't';

      long v = (((b > 300) ? (b + c) : 0) + (hot ? 29 : 0));
      hash ^= (v + 1);
      hash *= FNV64_PRIME;
    }

    return hash;
  }
}
