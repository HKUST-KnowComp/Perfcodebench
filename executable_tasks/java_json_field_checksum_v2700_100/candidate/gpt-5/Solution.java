import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int i = 5; // start after {"u":

      // Parse u (digits until comma)
      int u = 0;
      char ch;
      while (true) {
        ch = row.charAt(i++);
        if (ch == ',') break;
        u = u * 10 + (ch - '0');
      }

      // Skip "s":
      i += 4;

      // Parse s (digits until comma)
      int s = 0;
      while (true) {
        ch = row.charAt(i++);
        if (ch == ',') break;
        s = s * 10 + (ch - '0');
      }

      // Skip "f":
      i += 4;

      // Parse boolean: 't' for true, 'f' for false
      boolean f = row.charAt(i) == 't';

      int t = (u & 1023) + s + (f ? 17 : 0);
      long v = (long) t;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
