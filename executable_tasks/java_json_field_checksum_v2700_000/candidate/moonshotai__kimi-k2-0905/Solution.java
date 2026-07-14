import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            char[] cs = row.toCharArray();
            int u = 0, s = 0;
            boolean f = false;

            // scan "u":
            int p = 5;
            while (cs[p] != ',') {
                u = u * 10 + (cs[p++] - '0');
            }
            p += 5; // skip ,"s":
            while (cs[p] != ',') {
                s = s * 10 + (cs[p++] - '0');
            }
            p += 5; // skip ,"f":
            f = cs[p] == 't';

            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}