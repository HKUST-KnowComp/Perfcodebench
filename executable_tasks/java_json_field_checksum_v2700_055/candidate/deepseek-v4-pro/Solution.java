import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int u = 0, s = 0;
            boolean f;
            int i = 5; // after {"u":
            char c;
            while ((c = row.charAt(i)) != ',') {
                u = u * 10 + (c - '0');
                i++;
            }
            i += 5; // skip ,"s":
            while ((c = row.charAt(i)) != ',') {
                s = s * 10 + (c - '0');
                i++;
            }
            i += 5; // skip ,"f":
            f = row.charAt(i) == 't';
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}