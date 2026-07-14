import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int firstAmp = row.indexOf('&');
      int secondAmp = row.indexOf('&', firstAmp + 1);

      // parse u from index 2 to first '&'
      int u = 0;
      int i = 2;
      while (i < firstAmp) {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }

      // parse s from index (firstAmp+1+6) to second '&'
      int s = 0;
      i = firstAmp + 1 + 6;
      while (i < secondAmp) {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }

      // parse f from index (secondAmp+1+5) to end
      int f = 0;
      i = secondAmp + 1 + 5;
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