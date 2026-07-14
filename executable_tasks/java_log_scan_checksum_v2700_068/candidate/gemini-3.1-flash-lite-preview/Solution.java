import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Format: src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)
      int sIdx = row.indexOf("src=S") + 5;
      int lIdx = row.indexOf(" lat=", sIdx);
      int cIdx = row.indexOf(" code=", lIdx);
      int oIdx = row.indexOf(" ok=", cIdx);
      
      int src = Integer.parseInt(row.substring(sIdx, lIdx));
      int lat = Integer.parseInt(row.substring(lIdx + 5, cIdx));
      int code = Integer.parseInt(row.substring(cIdx + 6, oIdx));
      boolean ok = row.startsWith("yes", oIdx + 4);
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}