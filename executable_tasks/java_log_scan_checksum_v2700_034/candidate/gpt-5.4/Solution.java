import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);

      int p = 5; // after "src=S"
      int src = 0;
      char c;
      while ((c = row.charAt(p)) != ' ') {
        src = src * 10 + (c - '0');
        p++;
      }

      p += 5; // skip " lat="
      int lat = 0;
      while ((c = row.charAt(p)) != ' ') {
        lat = lat * 10 + (c - '0');
        p++;
      }

      p += 6; // skip " code="
      int code = 0;
      while ((c = row.charAt(p)) != ' ') {
        code = code * 10 + (c - '0');
        p++;
      }

      p += 4; // skip " ok="
      boolean ok = row.charAt(p) == 'y'; // "yes" or "no"

      long v = (src & 255) + (long) lat + (long) code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
