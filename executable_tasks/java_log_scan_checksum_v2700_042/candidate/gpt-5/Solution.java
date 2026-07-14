import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int i = 5; // position right after "src=S"

      int src = 0;
      char c;
      // parse src digits until space
      while (true) {
        c = s.charAt(i);
        if (c >= '0' && c <= '9') {
          src = src * 10 + (c - '0');
          i++;
        } else {
          break;
        }
      }

      // skip " lat="
      i += 5;

      int lat = 0;
      while (true) {
        c = s.charAt(i);
        if (c >= '0' && c <= '9') {
          lat = lat * 10 + (c - '0');
          i++;
        } else {
          break;
        }
      }

      // skip " code="
      i += 6;

      int code = 0;
      while (true) {
        c = s.charAt(i);
        if (c >= '0' && c <= '9') {
          code = code * 10 + (c - '0');
          i++;
        } else {
          break;
        }
      }

      // skip " ok="
      i += 4;

      // ok is either "yes" or "no"; check first char
      boolean ok = s.charAt(i) == 'y';

      int vInt = (src & 255) + lat + code + (ok ? 31 : 0);
      long v = (long) vInt;

      hash ^= (v + 1L);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}