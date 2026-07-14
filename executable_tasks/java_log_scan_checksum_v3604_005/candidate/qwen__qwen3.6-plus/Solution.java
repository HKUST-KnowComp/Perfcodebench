import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      int sp1 = row.indexOf(' ', 5);
      int sp2 = row.indexOf(' ', sp1 + 6);
      int sp3 = row.indexOf(' ', sp2 + 7);
      
      int src = 0;
      for (int i = 5; i < sp1; i++) src = src * 10 + (row.charAt(i) - '0');
      
      int lat = 0;
      for (int i = sp1 + 5; i < sp2; i++) lat = lat * 10 + (row.charAt(i) - '0');
      
      int code = 0;
      for (int i = sp2 + 6; i < sp3; i++) code = code * 10 + (row.charAt(i) - '0');
      
      boolean ok = row.charAt(sp3 + 4) == 'y';
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}