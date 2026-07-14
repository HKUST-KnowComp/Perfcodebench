import java.util.*;

public class Solution {

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0, n = rows.size(); i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      int idx = 5; // skip "src=S"
      
      // parse src
      int src = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
        src = src * 10 + (c - '0');
        idx++;
      }
      
      idx += 5; // skip " lat="
      
      // parse lat
      int lat = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
        lat = lat * 10 + (c - '0');
        idx++;
      }
      
      idx += 6; // skip " code="
      
      // parse code
      int code = 0;
      while (idx < len) {
        char c = row.charAt(idx);
        if (c < '0' || c > '9') break;
        code = code * 10 + (c - '0');
        idx++;
      }
      
      idx += 4; // skip " ok="
      
      // parse ok: "yes" or "no"
      boolean ok = (idx < len && row.charAt(idx) == 'y');
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
