import java.util.*;

public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = FNV64_OFFSET;
    for (int r = 0, n = rows.size(); r < n; r++) {
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

      // skip " lat=" (space + l a t =)
      i += 5;

      // parse lat
      int lat = 0;
      while (true) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        lat = lat * 10 + (ch - '0');
        i++;
      }

      // skip " code=" (space + c o d e =)
      i += 6;

      // parse code
      int code = 0;
      while (true) {
        char ch = s.charAt(i);
        if (ch < '0' || ch > '9') break;
        code = code * 10 + (ch - '0');
        i++;
      }

      // skip " ok=" (space + o k =)
      i += 4;

      // parse ok: 'y' for yes, 'n' for no
      boolean ok = s.charAt(i) == 'y';

      long v = (long) (src & 255) + (long) lat + (long) code + (ok ? 31L : 0L);
      hash ^= (v + 1L);
      hash *= FNV64_PRIME;
    }
    return hash;
  }
}
