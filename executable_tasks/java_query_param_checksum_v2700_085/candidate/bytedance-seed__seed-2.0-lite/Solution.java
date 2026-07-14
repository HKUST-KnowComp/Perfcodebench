import java.util.*;

public class Solution {
  private static int parseInt(String s, int start, int end) {
    int val = 0;
    for (int i = start; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return val;
  }

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int amp1 = row.indexOf('&');
      int amp2 = row.indexOf('&', amp1 + 1);
      int u = parseInt(row, 2, amp1);
      int s = parseInt(row, amp1 + 7, amp2);
      int f = parseInt(row, amp2 + 6, row.length());
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}