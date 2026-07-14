import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Expected format: {"u":(\d+),"s":(\d+),"f":(true|false),"tag":"x\d+"}
      // Find indices of key delimiters
      int uStart = row.indexOf(":", 4) + 1;
      int uEnd = row.indexOf(',', uStart);
      int sStart = row.indexOf(":", uEnd) + 1;
      int sEnd = row.indexOf(',', sStart);
      int fStart = row.indexOf(":", sEnd) + 1;
      
      int u = 0;
      for (int i = uStart; i < uEnd; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      
      int s = 0;
      for (int i = sStart; i < sEnd; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      
      boolean f = (row.charAt(fStart) == 't');
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}