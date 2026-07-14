import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int srcStart = 5;
      int latPos = row.indexOf(" lat=", srcStart);
      int srcEnd = latPos;
      int src = 0;
      for (int i = srcStart; i < srcEnd; i++) {
        src = src * 10 + (row.charAt(i) - '0');
      }

      int codePos = row.indexOf(" code=", latPos + 5);
      int latStart = latPos + 5;
      int latEnd = codePos;
      int lat = 0;
      for (int i = latStart; i < latEnd; i++) {
        lat = lat * 10 + (row.charAt(i) - '0');
      }

      int okPos = row.indexOf(" ok=", codePos + 6);
      int codeStart = codePos + 6;
      int codeEnd = okPos;
      int code = 0;
      for (int i = codeStart; i < codeEnd; i++) {
        code = code * 10 + (row.charAt(i) - '0');
      }

      boolean ok = row.charAt(okPos + 4) == 'y';

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}