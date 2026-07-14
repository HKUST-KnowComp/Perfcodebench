import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 5;
      // Parse u field from fixed JSON structure
      int u = 0;
      while (row.charAt(i) != ',') {
        u = u * 10 + (row.charAt(i) - '0');
        i++;
      }
      // Advance to first digit of s field
      i += 5;
      int s = 0;
      while (row.charAt(i) != ',') {
        s = s * 10 + (row.charAt(i) - '0');
        i++;
      }
      // Advance to boolean flag for f field
      i += 5;
      boolean f = row.charAt(i) == 't';
      // Exact same hash calculation as original implementation
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}