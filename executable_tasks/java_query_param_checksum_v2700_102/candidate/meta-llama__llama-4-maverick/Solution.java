import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int amp1 = row.indexOf('&');
      int amp2 = row.indexOf('&', amp1 + 1);
      int u = Integer.parseInt(row.substring(2, amp1));
      int s = Integer.parseInt(row.substring(amp1 + 7, amp2));
      int f = Integer.parseInt(row.substring(amp2 + 6));
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}