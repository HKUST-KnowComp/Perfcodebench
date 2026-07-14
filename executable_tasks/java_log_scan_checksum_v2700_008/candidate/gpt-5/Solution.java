import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;

    for (int r = 0, rn = rows.size(); r < rn; r++) {
      String row = rows.get(r);
      int n = row.length();

      // Expected format: "src=S<digits> lat=<digits> code=<digits> ok=<yes|no>"
      int i = 5; // after "src=S"

      // Parse src digits
      int src = 0;
      while (i < n) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        src = src * 10 + (c - '0');
        i++;
      }

      // Skip " lat="
      i++;      // space
      i += 4;   // 'lat='

      // Parse lat digits
      int lat = 0;
      while (i < n) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        lat = lat * 10 + (c - '0');
        i++;
      }

      // Skip " code="
      i++;      // space
      i += 5;   // 'code='

      // Parse code digits
      int code = 0;
      while (i < n) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        code = code * 10 + (c - '0');
        i++;
      }

      // Skip " ok="
      i++;      // space
      i += 3;   // 'ok='

      // Parse ok flag ('yes' or 'no')
      boolean ok = row.charAt(i) == 'y';

      int sum = (src & 255) + lat + code + (ok ? 31 : 0);
      long v = (long) sum;

      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }

    return hash;
  }
}