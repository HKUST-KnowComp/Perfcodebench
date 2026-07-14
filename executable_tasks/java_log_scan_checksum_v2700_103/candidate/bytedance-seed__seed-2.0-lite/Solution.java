import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Manual fixed-format parsing avoids regex/Matcher overhead
      int space1 = row.indexOf(' ', 5);
      int src = Integer.parseInt(row.substring(5, space1));
      
      int space2 = row.indexOf(' ', space1 + 1);
      int lat = Integer.parseInt(row.substring(space1 + 5, space2));
      
      int space3 = row.indexOf(' ', space2 + 1);
      int code = Integer.parseInt(row.substring(space2 + 6, space3));
      
      boolean ok = row.charAt(space3 + 4) == 'y';
      
      // Exact same hash calculation to preserve correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}