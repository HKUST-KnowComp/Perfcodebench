import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Locate only the commas needed to extract required fields
      int comma1 = row.indexOf(',');
      int comma2 = row.indexOf(',', comma1 + 1);
      int comma3 = row.indexOf(',', comma2 + 1);
      int comma4 = row.indexOf(',', comma3 + 1);
      
      // Parse b (original parts[2]) manually to avoid Integer.parseInt overhead
      int b = 0;
      for (int i = comma2 + 1; i < comma3; i++) {
        b = b * 10 + (row.charAt(i) - '0');
      }
      
      // Parse c (original parts[3]) manually
      int c = 0;
      for (int i = comma3 + 1; i < comma4; i++) {
        c = c * 10 + (row.charAt(i) - '0');
      }
      
      // Check hot flag directly via char access, no substring creation
      boolean hot = row.charAt(comma4 + 1) == 'h' 
        && row.charAt(comma4 + 2) == 'o' 
        && row.charAt(comma4 + 3) == 't';
      
      // Exact same calculation logic as baseline to preserve correctness
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}