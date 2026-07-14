import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String str = rows.get(r);
      int i = 5; // after '{', '"', 'u', '"', ':' -> start of u digits

      // parse u (\d+)
      int u = 0;
      char ch;
      while (true) {
        ch = str.charAt(i);
        if (ch >= '0' && ch <= '9') {
          u = u * 10 + (ch - '0');
          i++;
        } else {
          break; // expected ','
        }
      }

      // skip ,"s": (5 characters)
      i += 5;

      // parse s (\d+)
      int sVal = 0;
      while (true) {
        ch = str.charAt(i);
        if (ch >= '0' && ch <= '9') {
          sVal = sVal * 10 + (ch - '0');
          i++;
        } else {
          break; // expected ','
        }
      }

      // skip ,"f": (5 characters)
      i += 5;

      // parse f (true|false)
      boolean f = (str.charAt(i) == 't'); // 't' -> true, 'f' -> false

      // compute v with int arithmetic like baseline
      int vi = (u & 1023) + sVal + (f ? 17 : 0);
      long v = vi;

      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}