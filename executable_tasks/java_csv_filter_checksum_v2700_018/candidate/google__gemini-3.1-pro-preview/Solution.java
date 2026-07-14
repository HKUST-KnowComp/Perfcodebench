import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long multiplier = 1099511628211L;

    for (String row : rows) {
      int len = row.length();
      
      // Find the start of parts[2] (after 2nd comma)
      int commaCount = 0;
      int pos = 0;
      while (commaCount < 2 && pos < len) {
        if (row.charAt(pos++) == ',') {
          commaCount++;
        }
      }
      
      // Parse parts[2] (b)
      int b = 0;
      boolean negB = false;
      if (pos < len && row.charAt(pos) == '-') {
        negB = true;
        pos++;
      }
      while (pos < len) {
        char c = row.charAt(pos++);
        if (c == ',') break;
        b = b * 10 + (c - '0');
      }
      if (negB) b = -b;

      // Parse parts[3] (c)
      int cVal = 0;
      boolean negC = false;
      if (pos < len && row.charAt(pos) == '-') {
        negC = true;
        pos++;
      }
      while (pos < len) {
        char c = row.charAt(pos++);
        if (c == ',') break;
        cVal = cVal * 10 + (c - '0');
      }
      if (negC) cVal = -cVal;

      // Check parts[4] (hot)
      boolean hot = false;
      if (pos + 2 < len && 
          row.charAt(pos) == 'h' && 
          row.charAt(pos + 1) == 'o' && 
          row.charAt(pos + 2) == 't') {
        // Ensure it's exactly "hot" (either end of string or followed by comma)
        if (pos + 3 == len || row.charAt(pos + 3) == ',') {
          hot = true;
        }
      }

      long v = (b > 300 ? (long)b + cVal : 0L) + (hot ? 29L : 0L);
      hash ^= (v + 1);
      hash *= multiplier;
    }
    return hash;
  }
}