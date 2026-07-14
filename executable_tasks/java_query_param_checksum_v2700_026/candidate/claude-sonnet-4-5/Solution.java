import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Manual parsing to avoid split() and substring() allocations
      // Format: "u=<int>&status=<int>&flag=<int>"
      
      int idx1 = row.indexOf('&');
      int idx2 = row.indexOf('&', idx1 + 1);
      
      // Parse u (skip "u=")
      int u = parseInt(row, 2, idx1);
      
      // Parse s (skip "&status=")
      int s = parseInt(row, idx1 + 8, idx2);
      
      // Parse f (skip "&flag=")
      int f = parseInt(row, idx2 + 6, row.length());
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
  
  private static int parseInt(String s, int start, int end) {
    int result = 0;
    for (int i = start; i < end; i++) {
      result = result * 10 + (s.charAt(i) - '0');
    }
    return result;
  }
}