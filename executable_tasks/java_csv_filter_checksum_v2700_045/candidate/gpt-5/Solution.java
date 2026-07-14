import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int len = s.length();

      // Find comma boundaries quickly
      int p0 = s.indexOf(',');
      int p1 = s.indexOf(',', p0 + 1);
      int p2 = s.indexOf(',', p1 + 1);
      int p3 = s.indexOf(',', p2 + 1);
      int p4 = s.indexOf(',', p3 + 1);
      if (p4 == -1) p4 = len;

      // Parse integers from fields 2 and 3
      int b = parseInt(s, p1 + 1, p2);
      int c = parseInt(s, p2 + 1, p3);

      // Check if field 4 equals "hot"
      boolean hot = isHot(s, p3 + 1, p4);

      int temp = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      long v = temp;

      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }

    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int i = start;
    boolean neg = false;
    if (i < end) {
      char ch = s.charAt(i);
      if (ch == '-') { neg = true; i++; }
      else if (ch == '+') { i++; }
    }
    int val = 0;
    for (; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return neg ? -val : val;
  }

  private static boolean isHot(String s, int start, int end) {
    return (end - start) == 3 && s.charAt(start) == 'h' && s.charAt(start + 1) == 'o' && s.charAt(start + 2) == 't';
  }
}
