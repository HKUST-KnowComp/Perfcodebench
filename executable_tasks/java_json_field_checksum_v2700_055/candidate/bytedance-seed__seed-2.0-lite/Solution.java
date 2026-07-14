import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 5; // Skip opening prefix "{\"u\":" to reach first digit of u
      int u = 0;
      char c;
      while ((c = row.charAt(i)) != ',') {
        u = u * 10 + (c - '0');
        i++;
      }
      i += 6; // Skip separator ",\"s\":" to reach first digit of s
      int s = 0;
      while ((c = row.charAt(i)) != ',') {
        s = s * 10 + (c - '0');
        i++;
      }
      i += 6; // Skip separator ",\"f\":" to reach the boolean flag character
      boolean f = row.charAt(i) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}