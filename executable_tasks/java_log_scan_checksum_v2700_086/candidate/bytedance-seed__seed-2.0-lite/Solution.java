import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Locate space separators between the four fixed log fields
      int space1 = row.indexOf(' ');
      int space2 = row.indexOf(' ', space1 + 1);
      int space3 = row.indexOf(' ', space2 + 1);
      
      // Extract and parse all required fields identically to the original regex implementation
      int src = Integer.parseInt(row.substring(5, space1));
      int lat = Integer.parseInt(row.substring(space1 + 5, space2));
      int code = Integer.parseInt(row.substring(space2 + 6, space3));
      boolean ok = row.substring(space3 + 4).equals("yes");
      
      // Exact same checksum calculation as original to preserve correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}