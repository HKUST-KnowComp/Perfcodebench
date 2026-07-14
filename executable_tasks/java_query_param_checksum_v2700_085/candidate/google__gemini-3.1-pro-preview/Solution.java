import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    final long multiplier = 1099511628211L;

    for (String row : rows) {
      int len = row.length();
      
      // Parse 'u' starting at index 2
      int pos = 2;
      int u = 0;
      char c;
      while (pos < len && (c = row.charAt(pos)) != '&') {
        u = u * 10 + (c - '0');
        pos++;
      }
      
      // Skip '&score='
      pos += 7;
      int s = 0;
      while (pos < len && (c = row.charAt(pos)) != '&') {
        s = s * 10 + (c - '0');
        pos++;
      }
      
      // Skip '&flag='
      pos += 6;
      int f = 0;
      while (pos < len) {
        f = f * 10 + (row.charAt(pos) - '0');
        pos++;
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= multiplier;
    }
    return hash;
  }
}