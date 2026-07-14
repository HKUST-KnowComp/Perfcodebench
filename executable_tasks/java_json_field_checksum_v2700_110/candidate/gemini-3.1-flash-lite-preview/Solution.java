import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Expected format: {"u":123,"s":456,"f":true,"tag":"x789"}
      // Manual parsing is significantly faster than Regex
      int uStart = 6;
      int uEnd = row.indexOf(',', uStart);
      int u = Integer.parseInt(row.substring(uStart, uEnd));

      int sStart = uEnd + 6;
      int sEnd = row.indexOf(',', sStart);
      int s = Integer.parseInt(row.substring(sStart, sEnd));

      int fStart = sEnd + 6;
      boolean f = row.charAt(fStart) == 't';

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}