import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int idx = 1; // skip '{'
            // skip "u":
            idx += 4;
            // read u
            int u = 0;
            while (row.charAt(idx) != ',') {
                u = u * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            idx++; // skip comma
            // skip "s":
            idx += 4;
            int s = 0;
            while (row.charAt(idx) != ',') {
                s = s * 10 + (row.charAt(idx) - '0');
                idx++;
            }
            idx++; // skip comma
            // skip "f":
            idx += 4;
            boolean f = row.charAt(idx) == 't';
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}