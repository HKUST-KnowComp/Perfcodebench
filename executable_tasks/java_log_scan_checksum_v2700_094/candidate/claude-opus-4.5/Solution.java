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
      char c;
      while ((c = row.charAt(idx)) != ' ') {
        src = src * 10 + (c - '0');
        idx++;
      }
      
      // skip " lat="
      idx += 5;
      
      // parse lat
      int lat = 0;
      while ((c = row.charAt(idx)) != ' ') {
        lat = lat * 10 + (c - '0');
        idx++;
      }
      
      // skip " code="
      idx += 6;
      
      // parse code
      int code = 0;
      while ((c = row.charAt(idx)) != ' ') {
        code = code * 10 + (c - '0');
        idx++;
      }
      
      // skip " ok=", check if 'yes' (len - idx == 7 means "ok=yes")
      // remaining after space: "ok=yes" (6 chars) or "ok=no" (5 chars)
      boolean ok = (len - idx) == 7;
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}