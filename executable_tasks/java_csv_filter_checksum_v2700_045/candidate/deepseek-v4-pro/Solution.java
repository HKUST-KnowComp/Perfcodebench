import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final String hotStr = "hot";
        final long mul = 1099511628211L;
        for (String row : rows) {
            int c0 = row.indexOf(',');
            int c1 = row.indexOf(',', c0 + 1);
            int c2 = row.indexOf(',', c1 + 1);
            int c3 = row.indexOf(',', c2 + 1);
            int b = Integer.parseInt(row.substring(c1 + 1, c2));
            int c = Integer.parseInt(row.substring(c2 + 1, c3));
            boolean hot = row.length() - (c3 + 1) == 3 && row.regionMatches(c3 + 1, hotStr, 0, 3);
            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= mul;
        }
        return hash;
    }
}
