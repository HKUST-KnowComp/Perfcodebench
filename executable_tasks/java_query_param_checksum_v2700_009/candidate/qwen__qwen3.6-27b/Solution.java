import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int firstAmp = row.indexOf('&');
      int secondAmp = row.indexOf('&', firstAmp + 1);

      int u = 0;
      for (int i = 2; i < firstAmp; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }

      int s = 0;
      for (int i = firstAmp + 7; i < secondAmp; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }

      int f = 0;
      for (int i = secondAmp + 6; i < len; i++) {
        f = f * 10 + (row.charAt(i) - '0');
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}