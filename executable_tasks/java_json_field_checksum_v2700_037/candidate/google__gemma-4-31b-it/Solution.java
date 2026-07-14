import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            // Expected format: {"u":(digits),"s":(digits),"f":(true|false),"tag":"x(digits)"}
            
            int pos = 5; // Skip {"u":
            int u = 0;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                u = u * 10 + (row.charAt(pos++) - '0');
            }
            
            pos += 4; // Skip ,"s":
            int s = 0;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                s = s * 10 + (row.charAt(pos++) - '0');
            }
            
            pos += 4; // Skip ,"f":
            boolean f = row.charAt(pos) == 't';
            pos += (f ? 4 : 5); // Skip true or false
            
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}