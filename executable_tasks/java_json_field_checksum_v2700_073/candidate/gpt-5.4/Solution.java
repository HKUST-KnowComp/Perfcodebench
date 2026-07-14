import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;

    for (int idx = 0, n = rows.size(); idx < n; idx++) {
      String row = rows.get(idx);

      // Expected format:
      // {"u":<digits>,"s":<digits>,"f":true|false,"tag":"x<digits>"}
      // Parse only the needed fields using fixed offsets and digit scanning.

      int i = 5; // after {"u":
      int u = 0;
      char c;
      while ((c = row.charAt(i)) >= '0' && c <= '9') {
        u = u * 10 + (c - '0');
        i++;
      }

      // Skip ,"s":
      i += 6;
      int s = 0;
      while ((c = row.charAt(i)) >= '0' && c <= '9') {
        s = s * 10 + (c - '0');
        i++;
      }

      // Skip ,"f":
      i += 6;
      boolean f = row.charAt(i) == 't';

      long v = (u & 1023) + (long) s + (f ? 17L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }

    return hash;
  }
}
