import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Locate & separators directly to avoid split() overhead
      int firstAmp = -1;
      for (int i = 0; i < row.length(); i++) {
        if (row.charAt(i) == '&') {
          firstAmp = i;
          break;
        }
      }
      int secondAmp = -1;
      for (int i = firstAmp + 1; i < row.length(); i++) {
        if (row.charAt(i) == '&') {
          secondAmp = i;
          break;
        }
      }
      // Parse integers directly from original string without substrings
      int u = 0;
      for (int i = 2; i < firstAmp; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      int s = 0;
      int sStart = firstAmp + 1 + 6;
      for (int i = sStart; i < secondAmp; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      int f = 0;
      int fStart = secondAmp + 1 + 5;
      for (int i = fStart; i < row.length(); i++) {
        f = f * 10 + (row.charAt(i) - '0');
      }
      // Preserve original hash logic to maintain correctness
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}