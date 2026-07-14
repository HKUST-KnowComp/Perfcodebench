import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            // Parse first field: u=<digits>
            int uStart = row.indexOf('=') + 1;
            int uEnd = row.indexOf('&', uStart);
            int u = 0;
            for (int i = uStart; i < uEnd; i++) {
                u = u * 10 + (row.charAt(i) - '0');
            }
            // Parse second field: status=<digits>
            int sStart = row.indexOf('=', uEnd) + 1;
            int sEnd = row.indexOf('&', sStart);
            int s = 0;
            for (int i = sStart; i < sEnd; i++) {
                s = s * 10 + (row.charAt(i) - '0');
            }
            // Parse third field: flag=<digits>
            int fStart = row.indexOf('=', sEnd) + 1;
            int fEnd = len; // last field, no trailing &
            int f = 0;
            for (int i = fStart; i < fEnd; i++) {
                f = f * 10 + (row.charAt(i) - '0');
            }
            long v = (u & 2047) + s + f * 19L;
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}