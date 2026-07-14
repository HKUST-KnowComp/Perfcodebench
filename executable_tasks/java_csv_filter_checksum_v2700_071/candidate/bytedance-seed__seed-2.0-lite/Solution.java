import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Locate only the commas needed to extract required fields
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      int c5 = row.indexOf(',', c4 + 1);
      if (c5 == -1) c5 = row.length();
      
      // Manual fast parsing of integer fields
      int b = 0;
      for (int i = c2 + 1; i < c3; i++) {
        b = b * 10 + (row.charAt(i) - '0');
      }
      int c = 0;
      for (int i = c3 + 1; i < c4; i++) {
        c = c * 10 + (row.charAt(i) - '0');
      }
      
      // Check for 'hot' without substring creation
      boolean hot = false;
      if (c5 - c4 - 1 == 3) {
        hot = row.charAt(c4 + 1) == 'h' 
          && row.charAt(c4 + 2) == 'o' 
          && row.charAt(c4 + 3) == 't';
      }
      
      // Exact original calculation logic to preserve checksum correctness
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}