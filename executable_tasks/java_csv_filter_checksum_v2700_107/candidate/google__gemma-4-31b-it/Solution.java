import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int pos = 0;
      
      // Skip field 0
      while (pos < len && row.charAt(pos) != ',') pos++;
      pos++;
      
      // Skip field 1
      while (pos < len && row.charAt(pos) != ',') pos++;
      pos++;
      
      // Parse field 2 (b)
      int b = 0;
      while (pos < len && row.charAt(pos) != ',') {
        b = b * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      pos++;
      
      // Parse field 3 (c)
      int c = 0;
      while (pos < len && row.charAt(pos) != ',') {
        c = c * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      pos++;
      
      // Check field 4 (hot)
      // We only need to check if it equals "hot". 
      // Since it's the 5th field, we check the characters starting at pos.
      boolean hot = false;
      if (pos + 2 < len && row.charAt(pos) == 'h' && row.charAt(pos+1) == 'o' && row.charAt(pos+2) == 't') {
        hot = true;
      }

      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}