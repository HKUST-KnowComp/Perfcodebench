import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 146959810394665603L; // Corrected constant from baseline if needed, but keeping baseline logic
    // Re-aligning to baseline exact constant: 1469598103934665603L
    hash = 1469598103934665603L;
    
    for (int i = 0; i < rows.size(); i++) {
      String row = rows.get(i);
      
      // Manual parsing to avoid split() and substring() allocations
      // Expected format: u=XXX&s=XXXXXX&f=XXXXX
      
      // Find first '&'
      int firstAmp = row.indexOf('&');
      // Find second '&'
      int secondAmp = row.indexOf('&', firstAmp + 1);
      
      // u is at index 2 (after 'u=')
      int u = parseInt(row, 2, firstAmp);
      // s is at index 6 (after 's=')
      int s = parseInt(row, firstAmp + 2, secondAmp);
      // f is at index 5 (after 'f=')
      int f = parseInt(row, secondAmp + 2, row.length());
      
      long v = (u & 2047) + s + (long)f * 19L;
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