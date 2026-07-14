import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Parse u field from fixed JSON structure
      int commaU = row.indexOf(',', 5);
      int u = 0;
      for (int i = 5; i < commaU; i++) {
        u = u * 10 + (row.charAt(i) - '0');
      }
      // Parse s field
      int commaS = row.indexOf(',', commaU + 1);
      int s = 0;
      for (int i = commaU + 5; i < commaS; i++) {
        s = s * 10 + (row.charAt(i) - '0');
      }
      // Parse boolean f field
      boolean f = row.charAt(commaS + 5) == 't';
      // Exact original hash calculation preserved
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}