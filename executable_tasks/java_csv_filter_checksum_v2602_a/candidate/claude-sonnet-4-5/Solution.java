import java.util.*;

public class Solution {
  private static final long HASH_MULTIPLIER = 1099511628211L;
  
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    
    for (String row : rows) {
      int firstComma = row.indexOf(',');
      int secondComma = row.indexOf(',', firstComma + 1);
      int thirdComma = row.indexOf(',', secondComma + 1);
      int fourthComma = row.indexOf(',', thirdComma + 1);
      
      int b = parseInt(row, secondComma + 1, thirdComma);
      int c = parseInt(row, thirdComma + 1, fourthComma);
      boolean hot = row.charAt(fourthComma + 1) == 'h';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= HASH_MULTIPLIER;
    }
    return hash;
  }
  
  private static int parseInt(String s, int start, int end) {
    int result = 0;
    for (int i = start; i < end; i++) {
      result = result * 10 + (s.charAt(i) - '0');
    }
    return result;
  }
}