import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long multiplier = 1099511628211L;

    for (String row : rows) {
      // Find field boundaries manually to avoid String.split() overhead
      int c1 = row.indexOf(',');
      int c2 = row.indexOf(',', c1 + 1);
      int c3 = row.indexOf(',', c2 + 1);
      int c4 = row.indexOf(',', c3 + 1);
      int c5 = row.indexOf(',', c4 + 1);
      if (c5 == -1) {
        c5 = row.length();
      }

      // Parse field 2 (b) and field 3 (c) directly from the string
      int b = fastParseInt(row, c2 + 1, c3);
      int c = fastParseInt(row, c3 + 1, c4);

      // Check if field 4 is exactly "hot"
      boolean hot = (c5 - c4 == 4) && 
                    row.charAt(c4 + 1) == 'h' && 
                    row.charAt(c4 + 2) == 'o' && 
                    row.charAt(c4 + 3) == 't';

      // Calculate value based on original logic
      // Note: (b + c) is calculated as int to match baseline overflow behavior
      long v = (b > 300 ? (long)(b + c) : 0L) + (hot ? 29L : 0L);
      
      // Update FNV-1a style hash
      hash ^= (v + 1);
      hash *= multiplier;
    }
    return hash;
  }

  /**
   * Parses an integer from a substring without creating a new String object.
   */
  private static int fastParseInt(String s, int start, int end) {
    int num = 0;
    int i = start;
    boolean negative = false;
    if (i < end) {
      char first = s.charAt(i);
      if (first == '-') {
        negative = true;
        i++;
      } else if (first == '+') {
        i++;
      }
    }
    for (; i < end; i++) {
      num = num * 10 + (s.charAt(i) - '0');
    }
    return negative ? -num : num;
  }
}