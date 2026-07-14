import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int uColon = row.indexOf("\"u\":");
      int uStart = uColon + 4;
      int uEnd = row.indexOf(',', uStart);
      int u = 0;
      for (int i = uStart; i < uEnd; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }

      int sColon = row.indexOf("\"s\":", uEnd + 1);
      int sStart = sColon + 4;
      int sEnd = row.indexOf(',', sStart);
      int s = 0;
      for (int i = sStart; i < sEnd; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }

      int fColon = row.indexOf("\"f\":", sEnd + 1);
      int fStart = fColon + 4;
      boolean f = row.charAt(fStart) == 't';

      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}