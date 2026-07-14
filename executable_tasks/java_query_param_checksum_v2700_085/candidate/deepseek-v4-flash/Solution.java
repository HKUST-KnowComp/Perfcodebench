import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int amp1 = row.indexOf('&');
      int amp2 = row.indexOf('&', amp1 + 1);
      // parse u: indices [2, amp1)
      int u = 0;
      for (int i = 2; i < amp1; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      // parse s: indices [amp1+7, amp2)
      int s = 0;
      for (int i = amp1 + 7; i < amp2; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      // parse f: indices [amp2+6, row.length())
      int f = 0;
      for (int i = amp2 + 6; i < row.length(); i++) {
        f = f * 10 + (row.charAt(i) - '0');
      }
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}