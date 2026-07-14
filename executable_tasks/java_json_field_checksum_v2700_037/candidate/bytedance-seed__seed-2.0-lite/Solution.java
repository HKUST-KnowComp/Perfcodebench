import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 5; // Start of u's digits after leading "{\"u\":" sequence
      int u = 0;
      char c;
      while ((c = row.charAt(i)) != ',') {
        u = u * 10 + (c - '0');
        i++;
      }
      // Move past ",\"s\":" to start of s's digits
      i += 5;
      int s = 0;
      while ((c = row.charAt(i)) != ',') {
        s = s * 10 + (c - '0');
        i++;
      }
      // Move past ",\"f\":" to the boolean value
      i += 5;
      boolean f = row.charAt(i) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}