import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);

      // Expected format:
      // {"u":<digits>,"s":<digits>,"f":true|false,"tag":"x<digits>"}
      // Parse only the needed fields and skip validation work not required for checksum.
      int len = row.length();
      int p = 6; // after {"u":

      int u = 0;
      char c;
      while (p < len) {
        c = row.charAt(p);
        if (c == ',') break;
        u = u * 10 + (c - '0');
        p++;
      }

      p += 5; // skip ,"s":
      int s = 0;
      while (p < len) {
        c = row.charAt(p);
        if (c == ',') break;
        s = s * 10 + (c - '0');
        p++;
      }

      p += 5; // skip ,"f":
      boolean f = row.charAt(p) == 't';

      long v = (u & 1023) + (long) s + (f ? 17L : 0L);
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
