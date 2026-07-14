import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Locate fixed separators in the log line
      int idxLatSep = row.indexOf(" lat=");
      int idxCodeSep = row.indexOf(" code=", idxLatSep);
      int idxOkSep = row.indexOf(" ok=", idxCodeSep);
      
      // Parse src integer (starts after "src=S" prefix of length 5)
      int src = 0;
      for (int i = 5; i < idxLatSep; i++) {
        src = src * 10 + (row.charAt(i) - '0');
      }
      // Parse lat integer (starts after " lat=" separator of length 5)
      int lat = 0;
      for (int i = idxLatSep + 5; i < idxCodeSep; i++) {
        lat = lat * 10 + (row.charAt(i) - '0');
      }
      // Parse code integer (starts after " code=" separator of length 6)
      int code = 0;
      for (int i = idxCodeSep + 6; i < idxOkSep; i++) {
        code = code * 10 + (row.charAt(i) - '0');
      }
      // Check ok flag with single char access instead of string equals
      boolean ok = row.charAt(idxOkSep + 4) == 'y';
      
      // Exact same hash calculation as baseline to preserve correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}