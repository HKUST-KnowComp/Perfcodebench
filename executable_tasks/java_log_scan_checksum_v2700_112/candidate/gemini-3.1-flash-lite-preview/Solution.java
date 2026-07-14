import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Format: src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)
      int s1 = row.indexOf("S", 4);
      int s2 = row.indexOf(" ", s1);
      int src = Integer.parseInt(row.substring(s1 + 1, s2));

      int l1 = row.indexOf("lat=", s2) + 4;
      int l2 = row.indexOf(" ", l1);
      int lat = Integer.parseInt(row.substring(l1, l2));

      int c1 = row.indexOf("code=", l2) + 5;
      int c2 = row.indexOf(" ", c1);
      int code = Integer.parseInt(row.substring(c1, c2));

      int o1 = row.indexOf("ok=", c2) + 3;
      boolean ok = row.startsWith("yes", o1);

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}