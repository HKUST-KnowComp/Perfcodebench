import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Locate & delimiters directly to avoid split() overhead and intermediate objects
      int amp1 = row.indexOf('&');
      int amp2 = row.indexOf('&', amp1 + 1);
      
      // Manual inline integer parsing to avoid Integer.parseInt() and substring() overhead
      int u = 0;
      for (int i = 2; i < amp1; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      int s = 0;
      for (int i = amp1 + 7; i < amp2; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      int f = 0;
      for (int i = amp2 + 6; i < row.length(); i++) {
        f = f * 10 + (row.charAt(i) - '0');
      }
      
      // Exact original hash computation preserved for full correctness
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}