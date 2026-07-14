import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int pos = 5; // skip "src=S"
      
      // Parse src
      int src = 0;
      while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
        src = src * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      
      pos += 5; // skip " lat="
      
      // Parse lat
      int lat = 0;
      while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
        lat = lat * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      
      pos += 6; // skip " code="
      
      // Parse code
      int code = 0;
      while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
        code = code * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      
      pos += 4; // skip " ok="
      
      // Parse ok (check first char: 'y' for yes, 'n' for no)
      boolean ok = row.charAt(pos) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}