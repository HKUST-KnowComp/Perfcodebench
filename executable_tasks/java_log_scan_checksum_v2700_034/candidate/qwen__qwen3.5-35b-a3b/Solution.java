import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int srcStart = 5; // "src=S" length
      int srcEnd = row.indexOf(' ', srcStart);
      int src = Integer.parseInt(row.substring(srcStart, srcEnd));

      int latStart = srcEnd + 6; // " lat=" length
      int latEnd = row.indexOf(' ', latStart);
      int lat = Integer.parseInt(row.substring(latStart, latEnd));

      int codeStart = latEnd + 7; // " code=" length
      int codeEnd = row.indexOf(' ', codeStart);
      int code = Integer.parseInt(row.substring(codeStart, codeEnd));

      int okStart = codeEnd + 5; // " ok=" length
      boolean ok = row.charAt(okStart + 4) == 'e'; // "yes"[1] is 'e'

      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}