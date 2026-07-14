import java.util.*;

public class Solution {
  private static final long HASH_INIT = 1469598103934665603L;
  private static final long HASH_MULT = 1099511628211L;

  public static long run(List<String> rows) {
    long hash = HASH_INIT;
    for (String row : rows) {
      int len = row.length();
      int i = 5; // skip "src=S" (5 chars)

      // parse src
      int src = 0;
      while (i < len && row.charAt(i) != ' ') {
        src = src * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip space

      // skip "lat=" (4 chars)
      i += 4;

      // parse lat
      int lat = 0;
      while (i < len && row.charAt(i) != ' ') {
        lat = lat * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip space

      // skip "code=" (5 chars)
      i += 5;

      // parse code
      int code = 0;
      while (i < len && row.charAt(i) != ' ') {
        code = code * 10 + (row.charAt(i) - '0');
        i++;
      }
      i++; // skip space

      // skip "ok=" (3 chars)
      i += 3;

      // ok is 'y' (yes) or 'n' (no)
      boolean ok = (row.charAt(i) == 'y');

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= HASH_MULT;
    }
    return hash;
  }
}