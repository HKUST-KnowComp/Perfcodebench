import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx = 5; // skip "src=S"
      int src = 0;
      char ch;
      while ((ch = row.charAt(idx)) != ' ') {
        src = src * 10 + (ch - '0');
        idx++;
      }
      
      idx += 5; // skip " lat="
      int lat = 0;
      while ((ch = row.charAt(idx)) != ' ') {
        lat = lat * 10 + (ch - '0');
        idx++;
      }
      
      idx += 6; // skip " code="
      int code = 0;
      while ((ch = row.charAt(idx)) != ' ') {
        code = code * 10 + (ch - '0');
        idx++;
      }
      
      idx += 4; // skip " ok="
      boolean ok = row.charAt(idx) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
