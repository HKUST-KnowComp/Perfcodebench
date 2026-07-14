import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV64_OFFSET_BASIS;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int i = 5; // skip prefix: {"u":

      // parse u
      int u = 0;
      while (true) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        u = u * 10 + (c - '0');
        i++;
      }

      // skip ,"s": (comma + quote + s + quote + colon)
      i += 5;

      // parse s
      int s = 0;
      while (true) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        s = s * 10 + (c - '0');
        i++;
      }

      // skip ,"f": (comma + quote + f + quote + colon)
      i += 5;

      // parse f
      boolean f = row.charAt(i) == 't'; // 'true' or 'false'

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= FNV64_PRIME;
    }
    return hash;
  }
}
