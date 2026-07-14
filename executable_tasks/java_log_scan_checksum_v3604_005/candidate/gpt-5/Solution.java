import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (int ri = 0, rn = rows.size(); ri < rn; ri++) {
      String s = rows.get(ri);
      int len = s.length();
      int i = 5; // skip "src=S"

      // parse src digits; only need low 8 bits
      int src8 = 0;
      while (i < len) {
        char ch = s.charAt(i);
        if (ch >= '0' && ch <= '9') {
          src8 = ((src8 * 10) + (ch - '0')) & 255;
          i++;
        } else {
          break;
        }
      }

      // skip " lat="
      i++;      // space
      i += 4;   // 'lat='

      // parse lat
      int lat = 0;
      while (i < len) {
        char ch = s.charAt(i);
        if (ch >= '0' && ch <= '9') {
          lat = lat * 10 + (ch - '0');
          i++;
        } else {
          break;
        }
      }

      // skip " code="
      i++;      // space
      i += 5;   // 'code='

      // parse code
      int code = 0;
      while (i < len) {
        char ch = s.charAt(i);
        if (ch >= '0' && ch <= '9') {
          code = code * 10 + (ch - '0');
          i++;
        } else {
          break;
        }
      }

      // skip " ok="
      i++;      // space
      i += 3;   // 'ok='

      // parse yes/no
      boolean ok = s.charAt(i) == 'y';

      long v = (long) src8 + (long) lat + (long) code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= prime;
    }

    return hash;
  }
}
