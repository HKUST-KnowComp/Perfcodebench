import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int len = row.length();
      int u = 0, s = 0, f = 0;
      int idx = 0;
      char c;
      
      // Parse u
      while (row.charAt(idx) != '=') idx++;
      idx++;
      if (idx < len && row.charAt(idx) == '-') { idx++; while (idx < len && (c = row.charAt(idx)) >= '0' && c <= '9') { u = u * 10 - (c - '0'); idx++; } }
      else { while (idx < len && (c = row.charAt(idx)) >= '0' && c <= '9') { u = u * 10 + (c - '0'); idx++; } }
      
      // Parse s
      while (row.charAt(idx) != '=') idx++;
      idx++;
      if (idx < len && row.charAt(idx) == '-') { idx++; while (idx < len && (c = row.charAt(idx)) >= '0' && c <= '9') { s = s * 10 - (c - '0'); idx++; } }
      else { while (idx < len && (c = row.charAt(idx)) >= '0' && c <= '9') { s = s * 10 + (c - '0'); idx++; } }
      
      // Parse f
      while (row.charAt(idx) != '=') idx++;
      idx++;
      if (idx < len && row.charAt(idx) == '-') { idx++; while (idx < len && (c = row.charAt(idx)) >= '0' && c <= '9') { f = f * 10 - (c - '0'); idx++; } }
      else { while (idx < len && (c = row.charAt(idx)) >= '0' && c <= '9') { f = f * 10 + (c - '0'); idx++; } }
      
      long v = (u & 2047) + s + f * 19L;
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}