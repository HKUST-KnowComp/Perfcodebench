import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            // Manual parsing of {"u":digits,"s":digits,"f":true|false,...}
            int pos = 5; // start of u digits (after {"u":)
            int u = 0;
            char c;
            while ((c = row.charAt(pos)) != ',') {
                u = u * 10 + (c - '0');
                pos++;
            }
            pos += 5; // skip ,"s": to reach s digits
            int s = 0;
            while ((c = row.charAt(pos)) != ',') {
                s = s * 10 + (c - '0');
                pos++;
            }
            pos += 5; // skip ,"f": to reach t/f
            boolean f = row.charAt(pos) == 't';
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}