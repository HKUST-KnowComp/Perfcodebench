import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);
      int len = row.length();
      int i = 5; // skip "src=S"

      int src = 0;
      char ch;
      while (i < len && (ch = row.charAt(i)) >= '0' && ch <= '9') {
        src = src * 10 + (ch - '0');
        i++;
      }

      i += 5; // skip " lat="
      int lat = 0;
      while ((ch = row.charAt(i)) >= '0' && ch <= '9') {
        lat = lat * 10 + (ch - '0');
        i++;
      }

      i += 6; // skip " code="
      int code = 0;
      while ((ch = row.charAt(i)) >= '0' && ch <= '9') {
        code = code * 10 + (ch - '0');
        i++;
      }

      i += 4; // skip " ok="
      boolean ok = row.charAt(i) == 'y'; // 'yes' or 'no'

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}