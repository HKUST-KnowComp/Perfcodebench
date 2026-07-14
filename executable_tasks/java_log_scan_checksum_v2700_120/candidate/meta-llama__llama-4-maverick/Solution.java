import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int srcEnd = row.indexOf(' ');
      int src = Integer.parseInt(row.substring(4, srcEnd));
      int latStart = srcEnd + 5;
      int latEnd = row.indexOf(' ', latStart);
      int lat = Integer.parseInt(row.substring(latStart, latEnd));
      int codeStart = latEnd + 6;
      int codeEnd = row.indexOf(' ', codeStart);
      int code = Integer.parseInt(row.substring(codeStart, codeEnd));
      boolean ok = row.charAt(row.length() - 2) == 'y';
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}