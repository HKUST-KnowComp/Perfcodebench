import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Format: src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)
      int srcStart = 5;
      int latIdx = row.indexOf(" lat=");
      int codeIdx = row.indexOf(" code=", latIdx);
      int okIdx = row.indexOf(" ok=", codeIdx);
      
      int src = Integer.parseInt(row.substring(srcStart + 1, latIdx));
      int lat = Integer.parseInt(row.substring(latIdx + 5, codeIdx));
      int code = Integer.parseInt(row.substring(codeIdx + 6, okIdx));
      boolean ok = row.startsWith("yes", okIdx + 4);
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}