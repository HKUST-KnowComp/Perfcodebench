import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int idx = 0;
      int commas = 0;
      
      // Skip to field 2 (index 2)
      while (commas < 2 && idx < len) {
        if (row.charAt(idx) == ',') commas++;
        idx++;
      }
      
      // Parse b (field 2)
      int b = 0;
      boolean negB = false;
      if (idx < len && row.charAt(idx) == '-') { negB = true; idx++; }
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') break;
        b = b * 10 + (ch - '0');
        idx++;
      }
      if (negB) b = -b;
      if (idx < len) idx++;
      
      // Parse c (field 3)
      int c = 0;
      boolean negC = false;
      if (idx < len && row.charAt(idx) == '-') { negC = true; idx++; }
      while (idx < len) {
        char ch = row.charAt(idx);
        if (ch == ',') break;
        c = c * 10 + (ch - '0');
        idx++;
      }
      if (negC) c = -c;
      if (idx < len) idx++;
      
      // Check hot (field 4)
      boolean hot = false;
      if (idx + 3 <= len && 
          row.charAt(idx) == 'h' && 
          row.charAt(idx+1) == 'o' && 
          row.charAt(idx+2) == 't') {
        if (idx + 3 == len || row.charAt(idx+3) == ',') {
          hot = true;
        }
      }
      
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}