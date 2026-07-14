import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int i = 5; // Skip past "{\"u\":" (5 actual string chars: {,",u,",:)
      int u = 0;
      char c;
      while ((c = row.charAt(i)) != ',') {
        u = u * 10 + (c - '0');
        i++;
      }
      // Skip past ",\"s\":" (4 chars: ,",s,",:) to reach s's first digit
      i += 4;
      int s = 0;
      while ((c = row.charAt(i)) != ',') {
        s = s * 10 + (c - '0');
        i++;
      }
      // Skip past ",\"f\":" (4 chars) to reach boolean's first character
      i += 4;
      boolean f = row.charAt(i) == 't';
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}