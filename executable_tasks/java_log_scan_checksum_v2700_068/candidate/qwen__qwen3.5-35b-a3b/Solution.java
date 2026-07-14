import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      String[] parts = row.split(" ");
      
      // Parse src=S<digits>
      String srcPart = parts[0];
      int eqIndex = srcPart.indexOf('=');
      int src = Integer.parseInt(srcPart.substring(eqIndex + 2));
      
      // Parse lat=<digits>
      String latPart = parts[1];
      eqIndex = latPart.indexOf('=');
      int lat = Integer.parseInt(latPart.substring(eqIndex + 1));
      
      // Parse code=<digits>
      String codePart = parts[2];
      eqIndex = codePart.indexOf('=');
      int code = Integer.parseInt(codePart.substring(eqIndex + 1));
      
      // Parse ok=<yes|no>
      String okPart = parts[3];
      eqIndex = okPart.indexOf('=');
      boolean ok = okPart.substring(eqIndex + 1).equals("yes");
      
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}