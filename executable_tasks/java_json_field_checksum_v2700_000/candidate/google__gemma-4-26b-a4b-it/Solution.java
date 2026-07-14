import java.util.*;

public class Solution {

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            char[] chars = row.toCharArray();
            int n = chars.length;
            
            // Expected format: {"u":(\d+),"s":(\d+),"f":(true|false),"tag":"x\d+"}
            // We scan manually to avoid Regex overhead
            
            int u = 0;
            int s = 0;
            boolean f = false;
            
            int i = 0;
            // Skip to "u":
            while (i < n && chars[i] != 'u') i++;
            i += 3; // skip u":
            while (i < n && chars[i] >= '0' && chars[i] <= '9') {
                u = u * 10 + (chars[i] - '0');
                i++;
            }
            
            // Skip to "s":
            while (i < n && chars[i] != 's') i++;
            i += 3; // skip s":
            while (i < n && chars[i] >= '0' && chars[i] <= '9') {
                s = s * 10 + (chars[i] - '0');
                i++;
            }
            
            // Skip to "f":
            while (i < n && chars[i] != 'f') i++;
            i += 3; // skip f":
            if (chars[i] == 't') {
                f = true;
                i += 4; // skip true
            } else {
                f = false;
                i += 5; // skip false
            }
            
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}