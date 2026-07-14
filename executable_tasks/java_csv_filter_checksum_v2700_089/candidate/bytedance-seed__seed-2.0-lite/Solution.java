import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int commaCount = 0;
      int b = 0;
      int c = 0;
      boolean hot = false;
      int rowLen = row.length();
      
      for (int i = 0; i < rowLen; i++) {
        char ch = row.charAt(i);
        if (ch == ',') {
          commaCount++;
          if (commaCount == 4) {
            // Check if parts[4] is exactly "hot"
            if (i + 4 <= rowLen) {
              if (row.charAt(i+1) == 'h' && row.charAt(i+2) == 'o' && row.charAt(i+3) == 't') {
                if (i+4 == rowLen || row.charAt(i+4) == ',') {
                  hot = true;
                }
              }
            }
            // Early exit: all required values collected
            break;
          }
          continue;
        }
        // Parse numeric fields directly from character stream
        if (commaCount == 2) {
          b = b * 10 + (ch - '0');
        } else if (commaCount == 3) {
          c = c * 10 + (ch - '0');
        }
      }
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}