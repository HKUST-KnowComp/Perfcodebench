import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int j = 5;
      int u = 0;
      while (row.charAt(j) != ',') {
        u = u * 10 + (row.charAt(j) - '0');
        j++;
      }
      j += 5;
      int s = 0;
      while (row.charAt(j) != ',') {
        s = s * 10 + (row.charAt(j) - '0');
        j++;
      }
      j += 5;
      boolean f = row.charAt(j) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}