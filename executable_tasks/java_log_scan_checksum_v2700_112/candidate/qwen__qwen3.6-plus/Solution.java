import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int sp1 = row.indexOf(' ', 5);
      int sp2 = row.indexOf(' ', sp1 + 1);
      int sp3 = row.indexOf(' ', sp2 + 1);
      
      int src = 0;
      for (int j = 5; j < sp1; j++) src = src * 10 + (row.charAt(j) - '0');
      
      int lat = 0;
      for (int j = sp1 + 5; j < sp2; j++) lat = lat * 10 + (row.charAt(j) - '0');
      
      int code = 0;
      for (int j = sp2 + 6; j < sp3; j++) code = code * 10 + (row.charAt(j) - '0');
      
      boolean ok = row.charAt(sp3 + 4) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}