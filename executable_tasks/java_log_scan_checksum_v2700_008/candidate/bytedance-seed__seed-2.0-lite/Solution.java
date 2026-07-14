import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Parse src: extract digits after "src=S" up to first space
      int firstSpace = row.indexOf(' ', 5);
      int src = Integer.parseInt(row, 5, firstSpace, 10);
      
      // Parse lat: extract digits after " lat=" up to next space
      int secondSpace = row.indexOf(' ', firstSpace + 1);
      int lat = Integer.parseInt(row, firstSpace + 5, secondSpace, 10);
      
      // Parse code: extract digits after " code=" up to next space
      int thirdSpace = row.indexOf(' ', secondSpace + 1);
      int code = Integer.parseInt(row, secondSpace + 6, thirdSpace, 10);
      
      // Check ok flag directly via char comparison, no substring
      boolean ok = row.charAt(thirdSpace + 4) == 'y';
      
      // Exact original hash logic to preserve correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}