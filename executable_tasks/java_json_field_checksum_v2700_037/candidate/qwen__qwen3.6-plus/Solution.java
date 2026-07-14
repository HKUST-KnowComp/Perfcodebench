import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int i = 0;
            int len = row.length();
            
            while (i < len && row.charAt(i) != ':') i++;
            i++;
            
            int u = 0;
            while (i < len) {
                char c = row.charAt(i);
                if (c < '0' || c > '9') break;
                u = (u * 10 + (c - '0')) & 1023;
                i++;
            }
            
            while (i < len && row.charAt(i) != ':') i++;
            i++;
            
            int s = 0;
            while (i < len) {
                char c = row.charAt(i);
                if (c < '0' || c > '9') break;
                s = s * 10 + (c - '0');
                i++;
            }
            
            while (i < len && row.charAt(i) != ':') i++;
            i++;
            
            boolean f = (i < len && row.charAt(i) == 't');
            
            long v = u + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}