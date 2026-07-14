import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int i = 5; // skip {"u":

      // Parse u (digits until ',')
      int u = 0;
      while (true) {
        char c = s.charAt(i);
        if (c >= '0' && c <= '9') {
          u = u * 10 + (c - '0');
          i++;
        } else {
          break; // reached ','
        }
      }

      // Skip ,"s": (5 chars)
      i += 5;

      // Parse s (digits until ',')
      int sval = 0;
      while (true) {
        char c = s.charAt(i);
        if (c >= '0' && c <= '9') {
          sval = sval * 10 + (c - '0');
          i++;
        } else {
          break; // reached ','
        }
      }

      // Skip ,"f": (5 chars)
      i += 5;

      // Parse f (true|false)
      boolean f;
      char c = s.charAt(i);
      if (c == 't') { // true
        f = true;
        // advance past "true"
        i += 4;
      } else { // false
        f = false;
        // advance past "false"
        i += 5;
      }

      long v = (long)((u & 1023)) + (long)sval + (f ? 17L : 0L);
      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
