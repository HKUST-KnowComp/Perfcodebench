import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Format: src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)
      int s1 = row.indexOf('S', 4) + 1;
      int s2 = row.indexOf(' ', s1);
      int src = Integer.parseInt(row.substring(s1, s2));

      int l1 = row.indexOf('=', s2) + 1;
      int l2 = row.indexOf(' ', l1);
      int lat = Integer.parseInt(row.substring(l1, l2));

      int c1 = row.indexOf('=', l2) + 1;
      int c2 = row.indexOf(' ', c1);
      int code = Integer.parseInt(row.substring(c1, c2));

      int o1 = row.indexOf('=', c2) + 1;
      boolean ok = row.charAt(o1) == 'y';

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}