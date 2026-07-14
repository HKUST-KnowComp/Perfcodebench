import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int u = 0, s = 0, f = 0;
      int i = 0;
      char c;
      
      while (i < len && row.charAt(i) != '=') i++;
      i++;
      if (i < len && row.charAt(i) == '-') {
        i++;
        while (i < len && (c = row.charAt(i)) != '&') { u = u * 10 - (c - '0'); i++; }
      } else {
        while (i < len && (c = row.charAt(i)) != '&') { u = u * 10 + (c - '0'); i++; }
      }
      i++;
      
      while (i < len && row.charAt(i) != '=') i++;
      i++;
      if (i < len && row.charAt(i) == '-') {
        i++;
        while (i < len && (c = row.charAt(i)) != '&') { s = s * 10 - (c - '0'); i++; }
      } else {
        while (i < len && (c = row.charAt(i)) != '&') { s = s * 10 + (c - '0'); i++; }
      }
      i++;
      
      while (i < len && row.charAt(i) != '=') i++;
      i++;
      if (i < len && row.charAt(i) == '-') {
        i++;
        while (i < len) { f = f * 10 - (row.charAt(i) - '0'); i++; }
      } else {
        while (i < len) { f = f * 10 + (row.charAt(i) - '0'); i++; }
      }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}