import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Format: src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)
      int srcStart = 5;
      int sIdx = row.indexOf(' ', srcStart);
      int src = Integer.parseInt(row.substring(srcStart + 1, sIdx));

      int latStart = row.indexOf("lat=", sIdx) + 4;
      int lIdx = row.indexOf(' ', latStart);
      int lat = Integer.parseInt(row.substring(latStart, lIdx));

      int codeStart = row.indexOf("code=", lIdx) + 5;
      int cIdx = row.indexOf(' ', codeStart);
      int code = Integer.parseInt(row.substring(codeStart, cIdx));

      int okStart = row.indexOf("ok=", cIdx) + 3;
      boolean ok = row.startsWith("yes", okStart);

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}