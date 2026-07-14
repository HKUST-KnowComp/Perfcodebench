import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int len = s.length();
      int i = 5; // after "src=S"

      int src = 0;
      while (i < len) {
        char c = s.charAt(i);
        if (c == ' ') break;
        src = src * 10 + (c - '0');
        i++;
      }

      i += 5; // skip " lat="
      int lat = 0;
      while (i < len) {
        char c = s.charAt(i);
        if (c == ' ') break;
        lat = lat * 10 + (c - '0');
        i++;
      }

      i += 6; // skip " code="
      int code = 0;
      while (i < len) {
        char c = s.charAt(i);
        if (c == ' ') break;
        code = code * 10 + (c - '0');
        i++;
      }

      i += 4; // skip " ok="
      boolean ok = s.charAt(i) == 'y'; // "yes" or "no"

      long v = (src & 255) + (long) lat + (long) code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
