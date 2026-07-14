import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int amp1 = row.indexOf('&');
      int amp2 = row.indexOf('&', amp1 + 1);

      // u: parts[0].substring(2)
      int u = parseFastInt(row, 2, amp1);
      // s: parts[1].substring(6)
      int s = parseFastInt(row, amp1 + 7, amp2);
      // f: parts[2].substring(5)
      int f = parseFastInt(row, amp2 + 6, row.length());

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseFastInt(String s, int start, int end) {
    int res = 0;
    for (int i = start; i < end; i++) {
      res = res * 10 + (s.charAt(i) - '0');
    }
    return res;
  }
}