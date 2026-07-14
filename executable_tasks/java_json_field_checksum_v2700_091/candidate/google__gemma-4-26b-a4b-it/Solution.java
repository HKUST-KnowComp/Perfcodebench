import java.util.*;

public class Solution {

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            char[] chars = row.toCharArray();
            int n = chars.length;
            
            // Expected format: {"u":123,"s":456,"f":true,"tag":"x1"}
            // We scan for the specific positions of the values.
            
            int u = 0;
            int s = 0;
            boolean f = false;
            
            int i = 0;
            // Skip {"u":
            while (i < n && chars[i] != ':') i++;
            i++;
            // Parse u
            while (i < n && chars[i] >= '0' && chars[i] <= '9') {
                u = u * 10 + (chars[i] - '0');
                i++;
            }
            
            // Skip ,"s":
            while (i < n && chars[i] != ':') i++;
            i++;
            // Parse s
            while (i < n && chars[i] >= '0' && chars[i] <= '9') {
                s = s * 10 + (chars[i] - '0');
                i++;
            }
            
            // Skip ,"f":
            while (i < n && chars[i] != ':') i++;
            i++;
            // Parse f
            if (chars[i] == 't') {
                f = true;
                i += 4; // skip "true"
            } else {
                f = false;
                i += 5; // skip "false"
            }

            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}