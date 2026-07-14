import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;
    for (int idx = 0, n = rows.size(); idx < n; idx++) {
      String row = rows.get(idx);
      int i = 5; // start after {"u":
      int u = 0;
      char c;
      // parse u digits
      while ((c = row.charAt(i)) >= '0' && c <= '9') {
        u = u * 10 + (c - '0');
        i++;
      }
      // skip ,"s":
      i += 5;
      // parse s digits
      int s = 0;
      while ((c = row.charAt(i)) >= '0' && c <= '9') {
        s = s * 10 + (c - '0');
        i++;
      }
      // skip ,"f":
      i += 5;
      // parse boolean true/false
      boolean f;
      c = row.charAt(i);
      if (c == 't') {
        f = true;
        i += 4; // skip "true"
      } else {
        f = false;
        i += 5; // skip "false"
      }
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= prime;
    }
    return hash;
  }
}
