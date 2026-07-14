import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int comma1 = row.indexOf(',');
      int comma2 = row.indexOf(',', comma1 + 1);
      int comma3 = row.indexOf(',', comma2 + 1);
      int comma4 = row.indexOf(',', comma3 + 1);
      
      int b = parseInt(row, comma1 + 1, comma2);
      int c = parseInt(row, comma2 + 1, comma3);
      
      boolean hot = (comma4 - comma3 - 1 == 3) 
                    && row.charAt(comma3 + 1) == 'h' 
                    && row.charAt(comma3 + 2) == 'o' 
                    && row.charAt(comma3 + 3) == 't';
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseInt(String s, int start, int end) {
    int val = 0;
    for (int i = start; i < end; i++) {
      val = val * 10 + (s.charAt(i) - '0');
    }
    return val;
  }
}