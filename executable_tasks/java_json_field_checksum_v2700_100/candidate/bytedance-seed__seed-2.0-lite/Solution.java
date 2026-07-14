import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Parse u field: skip past opening "{\"u\":" prefix
      int i = 5;
      int u = 0;
      char c;
      while ((c = row.charAt(i++)) != ',') {
        u = u * 10 + (c - '0');
      }
      // Parse s field: skip past ",\"s\":" prefix
      i += 4;
      int s = 0;
      while ((c = row.charAt(i++)) != ',') {
        s = s * 10 + (c - '0');
      }
      // Parse f field: skip past ",\"f\":" prefix
      i += 4;
      boolean f = row.charAt(i) == 't';
      // Exact same hash calculation as baseline to preserve correctness
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}