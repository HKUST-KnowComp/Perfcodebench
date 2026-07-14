import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Solution {
  private static final Pattern P = Pattern.compile("^src=(\\d+) lat=(\\d+) code=(\\d+) ok=(yes|no)$");

  public static long run(List<String> rows) {
    long hash = 1469598103934665603L;
    for (String row : rows) {
      Matcher m = P.matcher(row);
      if (m.matches()) {
        int src = Integer.parseInt(m.group(1));
        int lat = Integer.parseInt(m.group(2));
        int code = Integer.parseInt(m.group(3));
        boolean ok = m.group(4).equals("yes");
        int v = (src & 255) + lat + code + (ok ? 31 : 0);
        hash ^= (v + 1);
        hash *= 1099511628211L;
      }
    }
    return hash;
  }
}