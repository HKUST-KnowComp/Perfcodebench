import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();

      // Fixed format: src=S<digits> lat=<digits> code=<digits> ok=(yes|no)
      // Prefix "src=S" is 5 chars.
      int p = 5;

      int src = 0;
      char c;
      while (p < len && (c = row.charAt(p)) >= '0' && c <= '9') {
        src = src * 10 + (c - '0');
        p++;
      }

      // Skip " lat="
      p += 5;

      int lat = 0;
      while (p < len && (c = row.charAt(p)) >= '0' && c <= '9') {
        lat = lat * 10 + (c - '0');
        p++;
      }

      // Skip " code="
      p += 6;

      int code = 0;
      while (p < len && (c = row.charAt(p)) >= '0' && c <= '9') {
        code = code * 10 + (c - '0');
        p++;
      }

      // Skip " ok="
      p += 4;

      boolean ok = row.charAt(p) == 'y'; // "yes" or "no"

      long v = (src & 255) + (long) lat + (long) code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
