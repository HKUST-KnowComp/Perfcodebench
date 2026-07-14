import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int amp1 = row.indexOf('&', 2);
      
      int u = 0;
      for (int i = 2; i < amp1; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      
      int sStart = amp1 + 7;
      int amp2 = row.indexOf('&', sStart);
      int s = 0;
      for (int i = sStart; i < amp2; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      
      int fStart = amp2 + 6;
      int f = 0;
      for (int i = fStart; i < len; i++) {
        f = f * 10 + (row.charAt(i) - '0');
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}