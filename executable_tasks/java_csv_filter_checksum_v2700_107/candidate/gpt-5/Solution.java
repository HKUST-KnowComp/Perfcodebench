import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long prime = 1099511628211L;
    for (int r = 0, size = rows.size(); r < size; r++) {
      String s = rows.get(r);
      int n = s.length();
      int i = 0;

      // Advance to the start of the 3rd field (index 2)
      int commas = 0;
      while (commas < 2 && i < n) {
        if (s.charAt(i) == ',') {
          commas++;
        }
        i++;
      }

      // Parse field 3 (parts[2]) as int b
      int b = 0;
      int sign = 1;
      if (i < n) {
        char ch = s.charAt(i);
        if (ch == '-') {
          sign = -1;
          i++;
        } else if (ch == '+') {
          i++;
        }
        while (i < n) {
          ch = s.charAt(i);
          if (ch == ',') break;
          b = b * 10 + (ch - '0');
          i++;
        }
        b *= sign;
      }

      // Skip comma separating to next field
      if (i < n && s.charAt(i) == ',') i++;

      // Parse field 4 (parts[3]) as int c
      int c = 0;
      sign = 1;
      if (i < n) {
        char ch = s.charAt(i);
        if (ch == '-') {
          sign = -1;
          i++;
        } else if (ch == '+') {
          i++;
        }
        while (i < n) {
          ch = s.charAt(i);
          if (ch == ',') break;
          c = c * 10 + (ch - '0');
          i++;
        }
        c *= sign;
      }

      // Skip comma to field 5 (parts[4])
      if (i < n && s.charAt(i) == ',') i++;

      // Check if field 5 equals "hot"
      int start = i;
      while (i < n && s.charAt(i) != ',') {
        i++;
      }
      int len = i - start;
      boolean hot = (len == 3
          && s.charAt(start) == 'h'
          && s.charAt(start + 1) == 'o'
          && s.charAt(start + 2) == 't');

      long v = ((b > 300) ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1L);
      hash *= prime;
    }
    return hash;
  }
}