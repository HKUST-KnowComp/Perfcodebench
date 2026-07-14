import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int u = 0;
            int s = 0;
            boolean f = false;
            
            int len = row.length();
            int pos = 0;
            
            // Expected format: {"u":(digits),"s":(digits),"f":(true|false),"tag":"x(digits)"}
            // Skip to "u":
            pos = row.indexOf("\"u\":", pos) + 5;
            
            // Parse u
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                u = u * 10 + (row.charAt(pos++) - '0');
            }
            
            // Skip to "s":
            pos = row.indexOf("\"s\":", pos) + 5;
            
            // Parse s
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                s = s * 10 + (row.charAt(pos++) - '0');
            }
            
            // Skip to "f":
            pos = row.indexOf("\"f\":", pos) + 5;
            
            // Parse f
            if (pos < len && row.charAt(pos) == 't') {
                f = true;
                pos += 4; // "true"
            } else {
                f = false;
                pos += 5; // "false"
            }
            
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}