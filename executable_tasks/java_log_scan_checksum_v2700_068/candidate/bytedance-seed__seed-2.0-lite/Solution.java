import java.util.List;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      // Parse src: after fixed "src=S" prefix (starts at index 5) up to first space delimiter
      int firstSpace = row.indexOf(' ', 5);
      int src = Integer.parseInt(row.substring(5, firstSpace));
      
      // Parse lat: after first space + fixed "lat=" prefix
      int latStart = firstSpace + 5;
      int secondSpace = row.indexOf(' ', latStart);
      int lat = Integer.parseInt(row.substring(latStart, secondSpace));
      
      // Parse code: after second space + fixed "code=" prefix
      int codeStart = secondSpace + 6;
      int thirdSpace = row.indexOf(' ', codeStart);
      int code = Integer.parseInt(row.substring(codeStart, thirdSpace));
      
      // Parse ok flag: check first character of the ok field value
      boolean ok = row.charAt(thirdSpace + 4) == 'y';
      
      // Exact same checksum calculation as original to preserve correctness
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}