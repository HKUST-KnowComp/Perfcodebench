import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;
    final int n = rows.size();
    for (int r = 0; r < n; r++) {
      String s = rows.get(r);
      int i = 5; // skip "src=S"

      // parse src (digits)
      int src = 0;
      while (true) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        src = src * 10 + (ch - '0');
        i++;
      }

      i++;      // skip space
      i += 4;   // skip "lat="

      // parse lat
      int lat = 0;
      while (true) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        lat = lat * 10 + (ch - '0');
        i++;
      }

      i++;      // skip space
      i += 5;   // skip "code="

      // parse code
      int code = 0;
      while (true) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        code = code * 10 + (ch - '0');
        i++;
      }

      i++;      // skip space
      i += 3;   // skip "ok="

      boolean ok = s.charAt(i) == 'y';
      // advance past yes/no; not needed further but keeps index consistent
      i += ok ? 3 : 2;

      int sum = (src & 255) + lat + code + (ok ? 31 : 0);
      long v = (long) sum;
      hash ^= (v + 1);
      hash *= prime;
    }
    return hash;
  }
}