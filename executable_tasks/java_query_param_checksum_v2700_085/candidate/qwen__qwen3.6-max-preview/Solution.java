import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int idx = 0, size = rows.size(); idx < size; idx++) {
      String row = rows.get(idx);
      int len = row.length();
      int i = 0;
      
      i += 2;
      int u = 0;
      boolean uNeg = i < len && row.charAt(i) == '-';
      if (uNeg) i++;
      while (i < len) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        u = u * 10 + (c - '0');
        i++;
      }
      if (uNeg) u = -u;
      
      i += 7;
      int s = 0;
      boolean sNeg = i < len && row.charAt(i) == '-';
      if (sNeg) i++;
      while (i < len) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        s = s * 10 + (c - '0');
        i++;
      }
      if (sNeg) s = -s;
      
      i += 6;
      int f = 0;
      boolean fNeg = i < len && row.charAt(i) == '-';
      if (fNeg) i++;
      while (i < len) {
        char c = row.charAt(i);
        if (c < '0' || c > '9') break;
        f = f * 10 + (c - '0');
        i++;
      }
      if (fNeg) f = -f;
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}