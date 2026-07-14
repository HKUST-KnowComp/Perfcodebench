import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Format: src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)
      int srcStart = row.indexOf("src=S") + 5;
      int latStart = row.indexOf(" lat=", srcStart) + 5;
      int codeStart = row.indexOf(" code=", latStart) + 6;
      int okStart = row.indexOf(" ok=", codeStart) + 4;

      int src = Integer.parseInt(row.substring(srcStart, row.indexOf(' ', srcStart)));
      int lat = Integer.parseInt(row.substring(latStart, row.indexOf(' ', latStart)));
      int code = Integer.parseInt(row.substring(codeStart, row.indexOf(' ', codeStart)));
      boolean ok = row.startsWith("yes", okStart);

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}