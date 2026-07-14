import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int u = 0;
      int s = 0;
      
      int pos = 5;
      char c;
      while ((c = row.charAt(pos)) != ',') {
        u = u * 10 + (c - '0');
        pos++;
      }
      
      pos += 5;
      while ((c = row.charAt(pos)) != ',') {
        s = s * 10 + (c - '0');
        pos++;
      }
      
      pos += 5;
      boolean f = row.charAt(pos) == 't';
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
