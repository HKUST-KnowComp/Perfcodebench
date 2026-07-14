import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int u = 0, s = 0;
            boolean f = false;
            
            int i = row.indexOf(':') + 1;
            while (true) {
                char c = row.charAt(i);
                if (c < '0' || c > '9') break;
                u = u * 10 + (c - '0');
                i++;
            }
            
            i = row.indexOf(':', i) + 1;
            while (true) {
                char c = row.charAt(i);
                if (c < '0' || c > '9') break;
                s = s * 10 + (c - '0');
                i++;
            }
            
            i = row.indexOf(':', i) + 1;
            f = row.charAt(i) == 't';
            
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}