import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int i = 5; // after {"u":

      // Parse u
      int u = 0;
      while (true) {
        char ch = row.charAt(i);
        if (ch < '0' || ch > '9') break;
        u = u * 10 + (ch - '0');
        i++;
      }

      // Skip ,"s":
      i += 5;

      // Parse s
      int s = 0;
      while (true) {
        char ch = row.charAt(i);
        if (ch < '0' || ch > '9') break;
        s = s * 10 + (ch - '0');
        i++;
      }

      // Skip ,"f":
      i += 5;

      // Parse f (true/false). We only need first char to decide.
      boolean f = row.charAt(i) == 't';

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
