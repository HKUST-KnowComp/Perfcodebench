import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (int r = 0, rn = rows.size(); r < rn; r++) {
      String row = rows.get(r);

      // Parse u starting at index 5 (after {"u":)
      int i = 5;
      int u = 0;
      char ch;
      while (true) {
        ch = row.charAt(i);
        if (ch == ',') break;
        u = u * 10 + (ch - '0');
        i++;
      }

      // Parse s starting at i + 5 (after ,"s":)
      int sIdx = i + 5;
      int s = 0;
      while (true) {
        ch = row.charAt(sIdx);
        if (ch == ',') break;
        s = s * 10 + (ch - '0');
        sIdx++;
      }

      // Parse f at sIdx + 5 (after ,"f":)
      boolean f = row.charAt(sIdx + 5) == 't';

      int vInt = (u & 1023) + s + (f ? 17 : 0);
      long v = (long) vInt;

      hash ^= (v + 1L);
      hash *= prime;
    }

    return hash;
  }
}
