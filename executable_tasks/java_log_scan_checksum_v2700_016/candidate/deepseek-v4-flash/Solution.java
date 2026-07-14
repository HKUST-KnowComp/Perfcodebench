import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int latIdx = row.indexOf(" lat=");
      int codeIdx = row.indexOf(" code=", latIdx + 1);
      int okIdx = row.indexOf(" ok=", codeIdx + 1);
      
      // parse src (starts at index 5)
      int src = 0;
      for (int i = 5; i < latIdx; i++) {
        src = src * 10 + (row.charAt(i) - '0');
      }
      
      // parse lat (starts after " lat=")
      int lat = 0;
      int latStart = latIdx + 5;
      for (int i = latStart; i < codeIdx; i++) {
        lat = lat * 10 + (row.charAt(i) - '0');
      }
      
      // parse code (starts after " code=")
      int code = 0;
      int codeStart = codeIdx + 6;
      for (int i = codeStart; i < okIdx; i++) {
        code = code * 10 + (row.charAt(i) - '0');
      }
      
      // ok: "yes" or "no"
      boolean ok = row.charAt(okIdx + 5) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}