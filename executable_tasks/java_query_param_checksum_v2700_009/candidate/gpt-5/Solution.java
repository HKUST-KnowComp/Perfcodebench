import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (int idx = 0, n = rows.size(); idx < n; idx++) {
      String row = rows.get(idx);
      int len = row.length();

      // Parse u after "u=" starting at index 2, until '&'
      int i = 2;
      int sign = 1;
      if (i < len) {
        char c = row.charAt(i);
        if (c == '-') { sign = -1; i++; }
        else if (c == '+') { i++; }
      }
      int uVal = 0;
      while (i < len) {
        char c = row.charAt(i);
        if (c == '&') break;
        uVal = uVal * 10 + (c - '0');
        i++;
      }
      int u = uVal * sign;

      if (i < len && row.charAt(i) == '&') i++;

      // Parse s after "score=" (6 chars)
      int j = i + 6;
      sign = 1;
      if (j < len) {
        char c = row.charAt(j);
        if (c == '-') { sign = -1; j++; }
        else if (c == '+') { j++; }
      }
      int sVal = 0;
      while (j < len) {
        char c = row.charAt(j);
        if (c == '&') break;
        sVal = sVal * 10 + (c - '0');
        j++;
      }
      int s = sVal * sign;

      if (j < len && row.charAt(j) == '&') j++;

      // Parse f after "flag=" (5 chars)
      int k = j + 5;
      sign = 1;
      if (k < len) {
        char c = row.charAt(k);
        if (c == '-') { sign = -1; k++; }
        else if (c == '+') { k++; }
      }
      int fVal = 0;
      while (k < len) {
        char c = row.charAt(k);
        if (c == '&') break;
        fVal = fVal * 10 + (c - '0');
        k++;
      }
      int f = fVal * sign;

      long v = (long)(u & 2047) + (long) s + (long) f * 19L;
      hash ^= (v + 1L);
      hash *= 1099511628211L;
    }
    return hash;
  }
}
