import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Format: src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)
      int srcStart = row.indexOf("src=S") + 5;
      int latStart = row.indexOf(" lat=", srcStart) + 6;
      int codeStart = row.indexOf(" code=", latStart) + 7;
      int okStart = row.indexOf(" ok=", codeStart) + 5;

      int srcEnd = row.indexOf(' ', srcStart);
      int latEnd = row.indexOf(' ', latStart);
      int codeEnd = row.indexOf(' ', codeStart);

      int src = Integer.parseInt(row.substring(srcStart, srcEnd));
      int lat = Integer.parseInt(row.substring(latStart, latEnd));
      int code = Integer.parseInt(row.substring(codeStart, codeEnd));
      boolean ok = row.startsWith("yes", okStart);

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}