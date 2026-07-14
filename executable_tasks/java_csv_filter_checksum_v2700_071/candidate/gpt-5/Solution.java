import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV64_OFFSET_BASIS;
    final int size = rows.size();
    for (int r = 0; r < size; r++) {
      String s = rows.get(r);
      int len = s.length();

      // Find positions of commas to locate fields 2, 3, and 4 quickly
      int p0 = s.indexOf(',');
      int p1 = s.indexOf(',', p0 + 1);
      int start2 = p1 + 1;
      int p2 = s.indexOf(',', start2);
      int start3 = p2 + 1;
      int p3 = s.indexOf(',', start3);
      int start4 = p3 + 1;
      int end4 = s.indexOf(',', start4);
      if (end4 < 0) end4 = len;

      // Parse integer b from [start2, p2)
      int i = start2;
      int sign = 1;
      int b = 0;
      if (i < p2) {
        char ch = s.charAt(i);
        if (ch == '-') { sign = -1; i++; }
        else if (ch == '+') { i++; }
        while (i < p2) {
          ch = s.charAt(i++);
          b = b * 10 + (ch - '0');
        }
        b *= sign;
      }

      // Parse integer c from [start3, p3)
      i = start3;
      sign = 1;
      int c = 0;
      if (i < p3) {
        char ch = s.charAt(i);
        if (ch == '-') { sign = -1; i++; }
        else if (ch == '+') { i++; }
        while (i < p3) {
          ch = s.charAt(i++);
          c = c * 10 + (ch - '0');
        }
        c *= sign;
      }

      // Check if field 4 equals "hot" exactly
      int f4len = end4 - start4;
      boolean hot = (f4len == 3
                     && s.charAt(start4) == 'h'
                     && s.charAt(start4 + 1) == 'o'
                     && s.charAt(start4 + 2) == 't');

      int tmp = 0;
      if (b > 300) tmp += (b + c);
      if (hot) tmp += 29;
      long v = (long) tmp;

      hash ^= (v + 1L);
      hash *= FNV64_PRIME;
    }
    return hash;
  }
}
