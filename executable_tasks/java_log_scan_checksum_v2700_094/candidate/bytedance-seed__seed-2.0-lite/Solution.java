import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int src = 0, lat = 0, code = 0;
      int i = 5; // Skip leading "src=S"
      char c;
      // Parse source ID
      while ((c = row.charAt(i)) != ' ') {
        src = src * 10 + (c - '0');
        i++;
      }
      // Skip space + "lat=" to reach latitude digits
      i += 5;
      // Parse latitude
      while ((c = row.charAt(i)) != ' ') {
        lat = lat * 10 + (c - '0');
        i++;
      }
      // Skip space + "code=" to reach code digits
      i += 6;
      // Parse code
      while ((c = row.charAt(i)) != ' ') {
        code = code * 10 + (c - '0');
        i++;
      }
      // Skip space + "ok=" to reach ok flag value
      i += 4;
      boolean ok = row.charAt(i) == 'y';
      // Exact original hash calculation to preserve correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}