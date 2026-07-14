import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int c1 = row.indexOf(',', 5);
      int u = 0;
      for (int j = 5; j < c1; j++) {
        u = u * 10 + (row.charAt(j) - '0');
      }
      
      int c2 = row.indexOf(',', c1 + 5);
      int s = 0;
      for (int j = c1 + 5; j < c2; j++) {
        s = s * 10 + (row.charAt(j) - '0');
      }
      
      boolean f = row.charAt(c2 + 5) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}