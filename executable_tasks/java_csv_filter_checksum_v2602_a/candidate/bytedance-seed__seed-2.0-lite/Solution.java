import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int commaCount = 0;
      int len = row.length();
      int b = 0;
      int c = 0;
      boolean hot = false;
      int pos = 0;

      // Skip to start of 3rd field (parts[2]) after 2 commas
      while (pos < len && commaCount < 2) {
        if (row.charAt(pos) == ',') commaCount++;
        pos++;
      }

      // Parse integer value of b (parts[2])
      while (pos < len && commaCount < 3) {
        char ch = row.charAt(pos);
        if (ch == ',') {
          commaCount++;
        } else {
          b = b * 10 + (ch - '0');
        }
        pos++;
      }

      // Parse integer value of c (parts[3])
      while (pos < len && commaCount < 4) {
        char ch = row.charAt(pos);
        if (ch == ',') {
          commaCount++;
        } else {
          c = c * 10 + (ch - '0');
        }
        pos++;
      }

      // Verify 5th field is exactly "hot" (matches parts[4].equals("hot"))
      if (pos + 2 < len) {
        if (row.charAt(pos) == 'h' && row.charAt(pos+1) == 'o' && row.charAt(pos+2) == 't') {
          if (pos + 3 == len || row.charAt(pos+3) == ',') {
            hot = true;
          }
        }
      }

      // Exact same calculation as original to preserve checksum correctness
      long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}