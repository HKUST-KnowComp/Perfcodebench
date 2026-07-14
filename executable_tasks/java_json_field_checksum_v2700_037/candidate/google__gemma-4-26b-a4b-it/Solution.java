import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      char[] chars = row.toCharArray();
      int n = chars.length;
      
      // Expected format: {"u":(\d+),"s":(\d+),"f":(true|false),"tag":"x\d+"}
      // We manually scan to avoid Regex overhead
      
      int i = 0;
      // Skip {"u":
      while (i < n && chars[i] != 'u') i++;
      i += 4; // skip u":
      
      long u = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        u = u * 10 + (chars[i] - '0');
        i++;
      }
      
      // Skip ,"s":
      while (i < n && chars[i] != 's') i++;
      i += 4; // skip s":
      
      long s = 0;
      while (i < n && chars[i] >= '0' && chars[i] <= '9') {
        s = s * 10 + (chars[i] - '0');
        i++;
      }
      
      // Skip ,"f":
      while (i < n && chars[i] != 'f') i++;
      i += 4; // skip f":
      
      boolean f = false;
      if (i + 4 <= n && chars[i] == 't' && chars[i+1] == 'r' && chars[i+2] == 'u' && chars[i+3] == 'e') {
        f = true;
        i += 4;
      } else {
        f = false;
        i += 5; // skip false
      }
      
      // The rest of the string (tag and closing brace) doesn't affect the checksum calculation
      // but we ensure we processed the required fields.
      
      long v = (u & 1023) + s + (f ? 17 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}