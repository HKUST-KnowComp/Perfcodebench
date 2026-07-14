import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int r = 0, n = rows.size(); r < n; r++) {
      String row = rows.get(r);

      // Parse u starting at index 5: {"u":
      int i = 5;
      int u = 0;
      char c;
      while (true) {
        c = row.charAt(i);
        if (c < '0' || c > '9') break;
        u = u * 10 + (c - '0');
        i++;
      }

      // Skip to s digits: ,"s": => advance 5 chars from the comma
      i += 5;
      int s = 0;
      while (true) {
        c = row.charAt(i);
        if (c < '0' || c > '9') break;
        s = s * 10 + (c - '0');
        i++;
      }

      // Skip to f value: ,"f": => advance 5 chars from the comma
      i += 5;
      boolean f = row.charAt(i) == 't'; // 't' for true, 'f' for false

      int val = (u & 1023) + s + (f ? 17 : 0);
      long v = val;
      hash ^= (v + 1);
      hash *= FNV_PRIME;
    }

    return hash;
  }
}
