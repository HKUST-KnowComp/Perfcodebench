import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV64_OFFSET;
    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int len = s.length();
      int i = 5; // skip "src=S"

      // parse src
      int src = 0;
      while (i < len) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        src = src * 10 + (ch - '0');
        i++;
      }

      i += 5; // skip " lat="
      int lat = 0;
      while (i < len) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        lat = lat * 10 + (ch - '0');
        i++;
      }

      i += 6; // skip " code="
      int code = 0;
      while (i < len) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        code = code * 10 + (ch - '0');
        i++;
      }

      i += 4; // skip " ok="
      boolean ok = s.charAt(i) == 'y'; // 'yes' vs 'no'

      int vInt = (src & 255) + lat + code + (ok ? 31 : 0);
      long v = (long) vInt;
      hash ^= (v + 1);
      hash *= FNV64_PRIME;
    }
    return hash;
  }
}
