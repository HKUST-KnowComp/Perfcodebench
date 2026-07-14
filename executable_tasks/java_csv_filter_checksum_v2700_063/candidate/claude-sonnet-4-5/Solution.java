import java.util.*;

public class Solution {
  private static final long PRIME = 1099511628211L;
  private static final long OFFSET = 1469598103934665603L;
  
  public static long run(List<String> rows) {
    long hash = OFFSET;
    
    for (String row : rows) {
      int len = row.length();
      int idx = 0;
      int commaCount = 0;
      int b = 0;
      int c = 0;
      boolean hot = false;
      
      // Parse manually without split
      for (int i = 0; i < len; i++) {
        char ch = row.charAt(i);
        if (ch == ',') {
          commaCount++;
          idx = i + 1;
        } else if (commaCount == 2) {
          // Parse field 2 (b)
          b = b * 10 + (ch - '0');
        } else if (commaCount == 3) {
          // Parse field 3 (c)
          c = c * 10 + (ch - '0');
        } else if (commaCount == 4 && ch == 'h') {
          // Field 4 starts with 'h' means "hot"
          hot = true;
        }
      }
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= PRIME;
    }
    
    return hash;
  }
}