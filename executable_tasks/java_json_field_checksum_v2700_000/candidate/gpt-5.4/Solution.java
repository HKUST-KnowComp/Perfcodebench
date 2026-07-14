import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);

      int idx = 6; // after {"u":
      int u = 0;
      char c;
      while ((c = row.charAt(idx)) != ',') {
        u = u * 10 + (c - '0');
        idx++;
      }

      idx += 5; // skip ,"s":
      int s = 0;
      while ((c = row.charAt(idx)) != ',') {
        s = s * 10 + (c - '0');
        idx++;
      }

      idx += 5; // skip ,"f":
      boolean f = row.charAt(idx) == 't';

      long v = (u & 1023L) + s + (f ? 17L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
