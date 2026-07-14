import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Format: "src=S" (5) + src + " lat=" (5) + lat + " code=" (6) + code + " ok=" (4) + ok
      int srcStart = 5;
      int latStart = row.indexOf(" lat=", srcStart);
      int codeStart = row.indexOf(" code=", latStart);
      int okStart = row.indexOf(" ok=", codeStart);

      int src = Integer.parseInt(row.substring(srcStart + 1, latStart));
      int lat = Integer.parseInt(row.substring(latStart + 5, codeStart));
      int code = Integer.parseInt(row.substring(codeStart + 6, okStart));
      boolean ok = row.regionMatches(okStart + 4, "yes", 0, 3);

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}