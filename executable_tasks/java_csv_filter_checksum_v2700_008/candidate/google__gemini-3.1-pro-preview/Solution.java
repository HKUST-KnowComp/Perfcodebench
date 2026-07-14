import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;

    for (String row : rows) {
      int len = row.length();
      
      // Find the positions of the commas to extract fields 2, 3, and 4
      // parts[0], parts[1], parts[2], parts[3], parts[4]
      int c1 = row.indexOf(',');
      if (c1 == -1) continue;
      int c2 = row.indexOf(',', c1 + 1);
      if (c2 == -1) continue;
      int c3 = row.indexOf(',', c2 + 1);
      if (c3 == -1) continue;
      int c4 = row.indexOf(',', c3 + 1);
      if (c4 == -1) continue;
      int c5 = row.indexOf(',', c4 + 1);
      if (c5 == -1) c5 = len;

      // Parse b (parts[2])
      int b = 0;
      for (int i = c2 + 1; i < c3; i++) {
        char ch = row.charAt(i);
        if (ch >= '0' && ch <= '9') {
          b = b * 10 + (ch - '0');
        }
      }

      // Parse c (parts[3])
      int c = 0;
      for (int i = c3 + 1; i < c4; i++) {
        char ch = row.charAt(i);
        if (ch >= '0' && ch <= '9') {
          c = c * 10 + (ch - '0');
        }
      }

      // Check if parts[4] is "hot"
      boolean hot = (c5 - c4 == 4) && 
                    row.charAt(c4 + 1) == 'h' && 
                    row.charAt(c4 + 2) == 'o' && 
                    row.charAt(c4 + 3) == 't';

      // Calculate v and update hash
      long v = (b > 300 ? (long)b + c : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1);
      hash *= prime;
    }
    
    return hash;
  }
}