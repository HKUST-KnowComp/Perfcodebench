import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 5; // Skip opening sequence `{"u":` (indices 0-4)
      int u = 0;
      char c;
      while ((c = row.charAt(i++)) != ',') {
        u = u * 10 + (c - '0');
      }
      // Move past `,"s":` to first digit of s
      i += 4;
      int s = 0;
      while ((c = row.charAt(i++)) != ',') {
        s = s * 10 + (c - '0');
      }
      // Move past `,"f":` to start of boolean value
      i += 4;
      boolean f = row.charAt(i) == 't';
      // Exact same checksum calculation as original
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}