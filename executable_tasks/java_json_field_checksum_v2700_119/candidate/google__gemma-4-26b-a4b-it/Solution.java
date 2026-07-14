import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      char[] chars = row.toCharArray();
      int n = chars.length;
      
      // Expected format: {"u":123,"s":456,"f":true,"tag":"x123"}
      // We scan for the specific keys to extract values.
      
      int uVal = 0;
      int sVal = 0;
      boolean fVal = false;

      int i = 0;
      while (i < n) {
        if (chars[i] == 'u' && i > 0 && chars[i-1] == '"' && chars[i+1] == '"' && chars[i+2] == ':') {
          i += 3;
          int start = i;
          while (i < n && chars[i] >= '0' && chars[i] <= '9') i++;
          for (int j = start; j < i; j++) {
            uVal = uVal * 10 + (chars[j] - '0');
          }
        } else if (chars[i] == 's' && i > 0 && chars[i-1] == '"' && chars[i+1] == '"' && chars[i+2] == ':') {
          i += 3;
          int start = i;
          while (i < n && chars[i] >= '0' && chars[i] <= '9') i++;
          for (int j = start; j < i; j++) {
            sVal = sVal * 10 + (chars[j] - '0');
          }
        } else if (chars[i] == 'f' && i > 0 && chars[i-1] == '"' && chars[i+1] == '"' && chars[i+2] == ':') {
          i += 3;
          if (chars[i] == 't') {
            fVal = true;
            i += 4;
          } else {
            fVal = false;
            i += 5;
          }
        } else {
          i++;
        }
      }

      long v = (uVal & 1023) + sVal + (fVal ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}