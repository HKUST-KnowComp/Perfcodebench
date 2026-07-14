import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int r = 0; r < n; r++) {
      String row = rows.get(r);
      int i = 5; // start of u digits after {"u":

      // parse u
      int u = 0;
      char ch;
      while ((ch = row.charAt(i)) >= '0' && ch <= '9') {
        u = u * 10 + (ch - '0');
        i++;
      }

      // skip ,"s": (5 characters)
      i += 5;

      // parse s
      int s = 0;
      while ((ch = row.charAt(i)) >= '0' && ch <= '9') {
        s = s * 10 + (ch - '0');
        i++;
      }

      // skip ,"f": (5 characters)
      i += 5;

      // parse f (true/false); first char is 't' or 'f'
      boolean f = row.charAt(i) == 't';

      long v = (long) (u & 1023) + (long) s + (f ? 17L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}