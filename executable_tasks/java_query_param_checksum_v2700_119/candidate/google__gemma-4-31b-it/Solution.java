import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int u = 0, s = 0, f = 0;
      int pos = 0;

      // Parse u: skip first 2 chars (e.g., "u=")
      pos += 2;
      while (pos < len && row.charAt(pos) != '&') {
        u = u * 10 + (row.charAt(pos++) - '0');
      }
      pos++; // skip '&'

      // Parse s: skip next 2 chars (e.g., "s=")
      // Note: baseline used substring(6), implying a fixed prefix like "param_s="
      // However, the baseline split("&") suggests the parts are separated by '&'.
      // Let's look at the baseline: parts[1].substring(6). 
      // This means the second part starts with 6 chars before the number.
      pos += 6;
      while (pos < len && row.charAt(pos) != '&') {
        s = s * 10 + (row.charAt(pos++) - '0');
      }
      pos++; // skip '&'

      // Parse f: skip next 5 chars (e.g., "param_f=")
      pos += 5;
      while (pos < len) {
        f = f * 10 + (row.charAt(pos++) - '0');
      }

      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}