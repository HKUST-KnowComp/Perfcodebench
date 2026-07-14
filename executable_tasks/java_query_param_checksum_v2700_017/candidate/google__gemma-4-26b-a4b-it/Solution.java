import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 146959810394665603L; // Corrected constant from baseline if needed, but keeping baseline logic
    // Re-evaluating baseline constant: 1469598103934665603L
    hash = 1469598103934665603L;
    
    for (int i = 0; i < rows.size(); i++) {
      String row = rows.get(i);
      
      // Manual parsing to avoid split() and substring() allocations
      // parts[0]: u (starts at index 2)
      // parts[1]: s (starts at index 6)
      // parts[2]: f (starts at index 5)
      
      int amp1 = row.indexOf('&');
      int amp2 = row.indexOf('&', amp1 + 1);
      
      // u: starts at index 2, ends at amp1
      int u = parseInt(row, 2, amp1);
      
      // s: starts at index amp1 + 1 + 6, ends at amp2
      // The baseline used parts[1].substring(6). 
      // parts[1] is the substring between amp1 and amp2.
      // So s starts at amp1 + 1 + 6
      int s = parseInt(row, amp1 + 7, amp2);
      
      // f: starts at index amp2 + 1 + 5
      // parts[2] is the substring after amp2.
      int f = parseInt(row, amp2 + 6, row.length());
      
      long v = (u & 2047) + s + (long) f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseInt(String s, int start, int end) {
    int num = 0;
    for (int i = start; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return num;
  }
}