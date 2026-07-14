import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int pos_lat = row.indexOf(" lat=");
      int pos_code = row.indexOf(" code=", pos_lat);
      int pos_ok = row.indexOf(" ok=", pos_code);
      
      int src = 0;
      for (int i = 5; i < pos_lat; i++) {
        src = src * 10 + (row.charAt(i) - '0');
      }
      
      int lat = 0;
      for (int i = pos_lat + 5; i < pos_code; i++) {
        lat = lat * 10 + (row.charAt(i) - '0');
      }
      
      int code = 0;
      for (int i = pos_code + 6; i < pos_ok; i++) {
        code = code * 10 + (row.charAt(i) - '0');
      }
      
      boolean ok = row.charAt(pos_ok + 4) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}