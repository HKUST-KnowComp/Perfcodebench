import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Parse src field (after "src=S" up to first space)
      int firstSpace = row.indexOf(' ');
      int src = Integer.parseInt(row.substring(5, firstSpace));
      // Parse lat field (after "lat=" up to second space)
      int secondSpace = row.indexOf(' ', firstSpace + 1);
      int lat = Integer.parseInt(row.substring(firstSpace + 5, secondSpace));
      // Parse code field (after "code=" up to third space)
      int thirdSpace = row.indexOf(' ', secondSpace + 1);
      int code = Integer.parseInt(row.substring(secondSpace + 6, thirdSpace));
      // Parse ok flag with a single char check to avoid substring/equals overhead
      boolean ok = row.charAt(thirdSpace + 4) == 'y';
      // Exact original hash logic preserved for checksum correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}