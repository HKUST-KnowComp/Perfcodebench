import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Start after fixed prefix "{\"u\":" (5 characters)
      int pos = 5;
      // Parse integer u
      int u = 0;
      while (row.charAt(pos) != ',') {
        u = u * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      // Skip past ",\"s\":" (5 characters) to start of s
      pos += 5;
      // Parse integer s
      int s = 0;
      while (row.charAt(pos) != ',') {
        s = s * 10 + (row.charAt(pos) - '0');
        pos++;
      }
      // Skip past ",\"f\":" (5 characters) to start of boolean flag
      pos += 5;
      boolean f = row.charAt(pos) == 't';
      // Exact same hash calculation as baseline to preserve correctness
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}