import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int firstAmp = row.indexOf('&');
      int secondAmp = row.indexOf('&', firstAmp + 1);
      
      // Parse u directly from original string
      int u = 0;
      for (int i = 2; i < firstAmp; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      
      // Parse s directly from original string
      int s = 0;
      int sStart = firstAmp + 7;
      for (int i = sStart; i < secondAmp; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      
      // Parse f directly from original string
      int f = 0;
      int fStart = secondAmp + 6;
      int rowLen = row.length();
      for (int i = fStart; i < rowLen; i++) {
        f = f * 10 + (row.charAt(i) - '0');
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}