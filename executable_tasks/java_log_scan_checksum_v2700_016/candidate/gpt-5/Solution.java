import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int i = 5; // skip "src=S"

      // Parse src
      int src = 0;
      while (true) {
        char c = s.charAt(i);
        if (c < '0' || c > '9') break;
        src = src * 10 + (c - '0');
        i++;
      }

      // Skip space and "lat="
      i++;      // space
      i += 4;   // "lat="

      // Parse lat
      int lat = 0;
      while (true) {
        char c = s.charAt(i);
        if (c < '0' || c > '9') break;
        lat = lat * 10 + (c - '0');
        i++;
      }

      // Skip space and "code="
      i++;      // space
      i += 5;   // "code="

      // Parse code
      int code = 0;
      while (true) {
        char c = s.charAt(i);
        if (c < '0' || c > '9') break;
        code = code * 10 + (c - '0');
        i++;
      }

      // Skip space and "ok="
      i++;      // space
      i += 3;   // "ok="

      // Parse ok ("yes" or "no")
      boolean ok = s.charAt(i) == 'y';

      long v = (src & 0xFF) + (long) lat + (long) code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
