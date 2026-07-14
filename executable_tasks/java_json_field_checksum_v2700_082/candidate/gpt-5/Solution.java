import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);
      int len = row.length();

      // Parsing assumes exact format: {"u":<u>,"s":<s>,"f":<true|false>,"tag":"x<digits>"}
      int idx = 5; // position after {"u":

      // parse u (digits until comma)
      int u = 0;
      while (true) {
        char ch = row.charAt(idx);
        int d = ch - '0';
        if (d < 0 || d > 9) break;
        u = u * 10 + d;
        idx++;
      }

      // skip ,"s": (comma + 4 chars)
      idx++;      // skip ','
      idx += 4;   // skip '"','s','"',':'

      // parse s (digits until comma)
      int s = 0;
      while (true) {
        char ch = row.charAt(idx);
        int d = ch - '0';
        if (d < 0 || d > 9) break;
        s = s * 10 + d;
        idx++;
      }

      // skip ,"f": (comma + 4 chars)
      idx++;      // skip ','
      idx += 4;   // skip '"','f','"',':'

      // parse boolean (true/false)
      boolean f = (row.charAt(idx) == 't');

      // Compute value exactly as baseline (int arithmetic, then widen to long)
      int vInt = (u & 1023) + s + (f ? 17 : 0);
      long v = vInt;
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
