import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int pos = 0;

      // first field: after first '=' up to first '&'
      int eq1 = row.indexOf('=', pos);
      int start1 = eq1 + 1;
      int amp1 = row.indexOf('&', start1);
      int u = parseInt(row, start1, amp1);

      // second field: after second '=' up to second '&'
      pos = amp1 + 1;
      int eq2 = row.indexOf('=', pos);
      int start2 = eq2 + 1;
      int amp2 = row.indexOf('&', start2);
      int s = parseInt(row, start2, amp2);

      // third field: after third '=' to end
      pos = amp2 + 1;
      int eq3 = row.indexOf('=', pos);
      int start3 = eq3 + 1;
      int f = parseInt(row, start3, len);

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int from, int to) {
    int value = 0;
    boolean negative = false;
    int i = from;
    if (i < to && s.charAt(i) == '-') {
      negative = true;
      i++;
    }
    for (; i < to; i++) {
      char c = s.charAt(i);
      value = value * 10 + (c - '0');
    }
    return negative ? -value : value;
  }
}