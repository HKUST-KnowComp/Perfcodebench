import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    final long FNV64_OFFSET = 1469598103934665603L;
    final long FNV64_PRIME = 1099511628211L;
    long hash = FNV64_OFFSET;

    for (int ri = 0, rn = rows.size(); ri < rn; ri++) {
      String row = rows.get(ri);
      int i = 5; // skip {"u":

      // parse u
      int u = 0;
      while (true) {
        char c = row.charAt(i);
        if (c >= '0' && c <= '9') {
          u = u * 10 + (c - '0');
          i++;
        } else {
          break; // should be comma
        }
      }

      // skip ,"s": (5 chars)
      i += 5;

      // parse s
      int s = 0;
      while (true) {
        char c = row.charAt(i);
        if (c >= '0' && c <= '9') {
          s = s * 10 + (c - '0');
          i++;
        } else {
          break; // should be comma
        }
      }

      // skip ,"f": (5 chars)
      i += 5;

      // parse boolean
      boolean flag = row.charAt(i) == 't'; // true or false

      long v = (u & 1023) + (long) s + (flag ? 17L : 0L);
      hash ^= (v + 1L);
      hash *= FNV64_PRIME;
    }

    return hash;
  }
}