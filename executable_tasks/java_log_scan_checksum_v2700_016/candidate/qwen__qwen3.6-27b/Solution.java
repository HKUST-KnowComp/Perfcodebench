import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int src = 0, lat = 0, code = 0;
      int i = 5; // skip "src=S"
      
      for (; i < len; i++) {
        char c = row.charAt(i);
        if (c == ' ') break;
        src = src * 10 + (c - '0');
      }
      i += 5; // skip " lat="
      
      for (; i < len; i++) {
        char c = row.charAt(i);
        if (c == ' ') break;
        lat = lat * 10 + (c - '0');
      }
      i += 6; // skip " code="
      
      for (; i < len; i++) {
        char c = row.charAt(i);
        if (c == ' ') break;
        code = code * 10 + (c - '0');
      }
      i += 4; // skip " ok="
      
      boolean ok = (i < len && row.charAt(i) == 'y');
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}