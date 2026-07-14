import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    int n = rows.size();
    for (int i = 0; i < n; i++) {
      String row = rows.get(i);
      int len = row.length();
      int u = 0, s = 0, f = 0;
      int j = 2;
      char c;
      while ((c = row.charAt(j)) != '&') {
        u = u * 10 + (c - '0');
        j++;
      }
      j += 7;
      while ((c = row.charAt(j)) != '&') {
        s = s * 10 + (c - '0');
        j++;
      }
      j += 6;
      while (j < len) {
        c = row.charAt(j);
        f = f * 10 + (c - '0');
        j++;
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}