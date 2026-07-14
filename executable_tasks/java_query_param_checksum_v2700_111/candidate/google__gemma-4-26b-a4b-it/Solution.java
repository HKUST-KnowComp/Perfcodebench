import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 146959810394665603L; // Corrected constant from baseline if needed, but keeping baseline logic
    // Re-aligning to exact baseline constant: 1469598103934665603L
    hash = 1469598103934665603L;
    
    for (int i = 0; i < rows.size(); i++) {
      String row = rows.get(i);
      
      // Find first '&'
      int firstAmp = row.indexOf('&');
      // Find second '&'
      int secondAmp = row.indexOf('&', firstAmp + 1);
      
      // u: parts[0].substring(2)
      int u = parseFastInt(row, 2, firstAmp);
      // s: parts[1].substring(6)
      int s = parseFastInt(row, firstAmp + 7, secondAmp);
      // f: parts[2].substring(5)
      int f = parseFastInt(row, secondAmp + 6, row.length());
      
      long v = (u & 2047) + s + (long) f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }

  private static int parseFastInt(String s, int start, int end) {
    int res = 0;
    for (int i = start; i < end; i++) {
      res = res * 10 + (s.charAt(i) - '0');
    }
    return res;
  }
}