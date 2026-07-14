import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < rows.size(); i++) {
      String row = rows.get(i);
      int idx = 5;
      int src = 0;
      char c;
      while ((c = row.charAt(idx)) >= '0' && c <= '9') {
        src = src * 10 + (c - '0');
        idx++;
      }
      idx += 5;
      int lat = 0;
      while ((c = row.charAt(idx)) >= '0' && c <= '9') {
        lat = lat * 10 + (c - '0');
        idx++;
      }
      idx += 6;
      int code = 0;
      while ((c = row.charAt(idx)) >= '0' && c <= '9') {
        code = code * 10 + (c - '0');
        idx++;
      }
      idx += 4;
      boolean ok = row.charAt(idx) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}