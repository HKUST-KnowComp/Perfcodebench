import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int idx = 0; idx < n; idx++) {
      String row = rows.get(idx);

      // Expected format:
      // {"u":<digits>,"s":<digits>,"f":true|false,"tag":"x<digits>"}
      // Parse only the needed fields and avoid regex / substring allocations.
      int i = 6; // after {"u":

      int u = 0;
      char c;
      while ((c = row.charAt(i)) != ',') {
        u = u * 10 + (c - '0');
        i++;
      }

      i += 5; // skip ,"s":
      int s = 0;
      while ((c = row.charAt(i)) != ',') {
        s = s * 10 + (c - '0');
        i++;
      }

      i += 5; // skip ,"f":
      boolean f = row.charAt(i) == 't';

      long v = (u & 1023) + (long) s + (f ? 17L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
