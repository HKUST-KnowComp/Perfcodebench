import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Manual parsing to avoid split() and substring() allocations
      // Format: "u=<int>&status=<int>&flag=<int>"
      
      // Find u value (after "u=")
      int idx = 2; // skip "u="
      int ampIdx = row.indexOf('&', idx);
      int u = parseInt(row, idx, ampIdx);
      
      // Find s value (after "status=")
      idx = ampIdx + 8; // skip "&status="
      ampIdx = row.indexOf('&', idx);
      int s = parseInt(row, idx, ampIdx);
      
      // Find f value (after "flag=")
      idx = ampIdx + 6; // skip "&flag="
      int f = parseInt(row, idx, row.length());
      
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