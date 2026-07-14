import java.util.*;

public class Solution {
  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      String[] parts = row.split(" ");
      String[] srcParts = parts[0].split("=");
      int src = Integer.parseInt(srcParts[1].substring(1));
      String[] latParts = parts[1].split("=");
      int lat = Integer.parseInt(latParts[1]);
      String[] codeParts = parts[2].split("=");
      int code = Integer.parseInt(codeParts[1]);
      String[] okParts = parts[3].split("=");
      boolean ok = okParts[1].equals("yes");
      long v = (src & 255) + lat + code + (ok ? 31 : 0);
      hash ^= (v + 1);
      hash *= 1099511628211L;
    }
    return hash;
  }
}