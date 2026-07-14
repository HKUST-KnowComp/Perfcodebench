import java.util.*;

public class Solution {
  private static final long HASH_MULTIPLIER = 1099511628211L;
  
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    
    for (String row : rows) {
      // Manual parsing to avoid split() allocation
      int firstComma = row.indexOf(',');
      int secondComma = row.indexOf(',', firstComma + 1);
      int thirdComma = row.indexOf(',', secondComma + 1);
      int fourthComma = row.indexOf(',', thirdComma + 1);
      
      // Parse field 2 (index 2, between secondComma and thirdComma)
      int b = 0;
      for (int i = secondComma + 1; i < thirdComma; i++) {
        b = b * 10 + (row.charAt(i) - '0');
      }
      
      // Parse field 3 (index 3, between thirdComma and fourthComma)
      int c = 0;
      for (int i = thirdComma + 1; i < fourthComma; i++) {
        c = c * 10 + (row.charAt(i) - '0');
      }
      
      // Check field 4 (index 4, after fourthComma) - check first char only
      boolean hot = row.charAt(fourthComma + 1) == 'h';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= HASH_MULTIPLIER;
    }
    
    return hash;
  }
}