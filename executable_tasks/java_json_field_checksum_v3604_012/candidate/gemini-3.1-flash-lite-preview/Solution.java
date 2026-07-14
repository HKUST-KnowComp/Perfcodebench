import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Expected format: {"u":(\d+),"s":(\d+),"f":(true|false),"tag":"x\d+"}
      // Manual parsing to avoid regex overhead
      int uStart = row.indexOf(":") + 1;
      int uEnd = row.indexOf(",", uStart);
      int u = Integer.parseInt(row.substring(uStart, uEnd));

      int sStart = row.indexOf(":", uEnd) + 1;
      int sEnd = row.indexOf(",", sStart);
      int s = Integer.parseInt(row.substring(sStart, sEnd));

      int fStart = row.indexOf(":", sEnd) + 1;
      boolean f = row.charAt(fStart) == 't';

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}