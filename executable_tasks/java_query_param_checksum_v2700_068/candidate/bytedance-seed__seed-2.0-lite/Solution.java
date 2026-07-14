import java.util.*;

public class Solution {
  private static int parseNumber(String s, int start, int end) {
    int num = 0;
    for (int i = start; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num;
  }

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int amp1 = -1, amp2 = -1;
      final int len = row.length();
      for (int i = 0; i < len; i++) {
        if (row.charAt(i) == '&') {
          if (amp1 == -1) {
            amp1 = i;
          } else {
            amp2 = i;
            break;
          }
        }
      }
      int u = parseNumber(row, 2, amp1);
      int s = parseNumber(row, amp1 + 7, amp2);
      int f = parseNumber(row, amp2 + 6, len);
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}