import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int i = 0;
      
      // Find first &
      int firstAmp = row.indexOf('&', i);
      int uStart = i + 2;
      int uEnd = firstAmp;
      int u = 0;
      for (int k = uStart; k < uEnd; k++) {
        u = u * 10 + (row.charAt(k) - '0');
      }
      
      i = firstAmp + 1;
      
      // Find second &
      int secondAmp = row.indexOf('&', i);
      int sStart = i + 6;
      int sEnd = secondAmp;
      int s = 0;
      for (int k = sStart; k < sEnd; k++) {
        s = s * 10 + (row.charAt(k) - '0');
      }
      
      i = secondAmp + 1;
      
      // Parse f
      int fStart = i + 5;
      int fEnd = len;
      int f = 0;
      for (int k = fStart; k < fEnd; k++) {
        f = f * 10 + (row.charAt(k) - '0');
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}