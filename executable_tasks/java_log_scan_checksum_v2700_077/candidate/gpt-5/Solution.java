import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String s = rows.get(r);
      int i = 5; // after "src=S"
      char c;

      // parse src number until space
      int src = 0;
      while ((c = s.charAt(i)) != ' ') {
        src = src * 10 + (c - '0');
        i++;
      }
      i++; // skip space

      // skip "lat=" (length 4)
      i += 4;
      int lat = 0;
      while ((c = s.charAt(i)) != ' ') {
        lat = lat * 10 + (c - '0');
        i++;
      }
      i++; // skip space

      // skip "code=" (length 5)
      i += 5;
      int code = 0;
      while ((c = s.charAt(i)) != ' ') {
        code = code * 10 + (c - '0');
        i++;
      }
      i++; // skip space

      // skip "ok=" (length 3)
      i += 3;
      boolean ok = s.charAt(i) == 'y';

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}