import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);

      int i = 5; // skip {"u":

      int u = 0;
      while (true) {
        char ch = row.charAt(i);
        if (ch >= '0' && ch <= '9') {
          u = u * 10 + (ch - '0');
          i++;
        } else {
          break; // reached ','
        }
      }

      // skip ,"s":
      i += 5;

      int s = 0;
      while (true) {
        char ch = row.charAt(i);
        if (ch >= '0' && ch <= '9') {
          s = s * 10 + (ch - '0');
          i++;
        } else {
          break; // reached ','
        }
      }

      // skip ,"f":
      i += 5;

      boolean f = row.charAt(i) == 't'; // true or false

      long v = (long)(u & 1023) + (long)s + (f ? 17L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
