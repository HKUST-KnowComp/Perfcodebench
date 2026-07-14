import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      // Find comma positions for fields 0,1,2,3,4
      int c0 = row.indexOf(',');
      int c1 = row.indexOf(',', c0 + 1);
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);

      // Parse b (field 2) and c (field 3) without substring allocation
      int b = parseInt(row, c1 + 1, c2);
      int c = parseInt(row, c2 + 1, c3);

      // Check if field 4 is exactly "hot" without substring
      int hotStart = c3 + 1;
      int hotEnd = (c4 == -1) ? len : c4;
      boolean hot = (hotEnd - hotStart == 3)
          && row.charAt(hotStart) == 'h'
          && row.charAt(hotStart + 1) == 'o'
          && row.charAt(hotStart + 2) == 't';

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int result = 0;
    for (int i = start; i < end; i++) {
      result = result * 10 + (s.charAt(i) - '0');
    }
    return result;
  }
}