import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int u = 0, s = 0, f = 0;
      int i = 0;
      
      // Skip 'u=' (index 0, 1)
      i = 2;
      while (i < len && row.charAt(i) != '&') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      
      // Skip '&s=' (index i, i+1, i+2, i+3, i+4, i+5)
      i += 6;
      while (i < len && row.charAt(i) != '&') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      
      // Skip '&f=' (index i, i+1, i+2, i+3, i+4)
      i += 5;
      while (i < len) {
        f = f * 10 + (row.charAt(i) - '0');
        i++;
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}