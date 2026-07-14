import java.util.*;

public class Solution {

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      final char[] cs = row.toCharArray();

      /* u value */
      int p = 6;                   // "{\"u\":" next char after :
      int u = 0;
      char c;
      while ((c = cs[p++]) != ',') {
        u = u * 10 + (c - '0');
      }

      /* s value */
      p++;                         // skip \n"s"
      int s = 0;
      while ((c = cs[p++]) != ',') {
        s = s * 10 + (c - '0');
      }

      /* f value */
      p += 6;                      // skip ,\"f\":
      boolean f = cs[p] == 't';  // first char decides

      /* compose v */
      int v = (u & 1023) + s + (f ? 17 : 0);

      /* update hash */
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}