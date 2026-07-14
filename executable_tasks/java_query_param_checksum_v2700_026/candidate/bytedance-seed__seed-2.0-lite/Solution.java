import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final long multiplier = 1099511628211L;
        for (String row : rows) {
            int len = row.length();
            int idx = 2;
            int u = 0;
            while (idx < len && row.charAt(idx) != '&') {
                u = u * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            idx += 6;
            int s = 0;
            while (idx < len && row.charAt(idx) != '&') {
                s = s * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            idx += 5;
            int f = 0;
            while (idx < len) {
                f = f * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            long v = (u & 2047) + s + f * 19L;
            hash ^= (v + 1);
            hash *= multiplier;
        }
        return hash;
    }
}