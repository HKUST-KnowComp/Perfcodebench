import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int k = 0, n = rows.size(); k < n; k++) {
      String row = rows.get(k);
      int len = row.length();
      int i = 5; // skip "src=S"
      
      int src = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == ' ') break;
        src = src * 10 + (c - '0');
        i++;
      }
      
      i += 5; // skip " lat="
      int lat = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == ' ') break;
        lat = lat * 10 + (c - '0');
        i++;
      }
      
      i += 6; // skip " code="
      int code = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == ' ') break;
        code = code * 10 + (c - '0');
        i++;
      }
      
      i += 4; // skip " ok="
      boolean ok = (i < len) && row.charAt(i) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}