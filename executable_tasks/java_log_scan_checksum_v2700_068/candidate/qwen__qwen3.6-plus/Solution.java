import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int idx1 = row.indexOf(' ', 5);
      int idx2 = row.indexOf(' ', idx1 + 5);
      int idx3 = row.indexOf(' ', idx2 + 6);
      
      int src = 0;
      for (int j = 5; j < idx1; j++) src = src * 10 + (row.charAt(j) - '0');
      
      int lat = 0;
      for (int j = idx1 + 5; j < idx2; j++) lat = lat * 10 + (row.charAt(j) - '0');
      
      int code = 0;
      for (int j = idx2 + 6; j < idx3; j++) code = code * 10 + (row.charAt(j) - '0');
      
      boolean ok = row.charAt(idx3 + 4) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}