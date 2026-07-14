import java.util.*;

public class Solution {

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            char[] chars = row.toCharArray();
            int n = chars.length;
            
            // Expected format: {"u":(\d+),"s":(\d+),"f":(true|false),"tag":"x\d+"}
            // We manually scan to avoid Regex overhead.
            
            int idx = 0;
            // Skip {"u":
            idx += 5;
            
            // Parse u
            long u = 0;
            while (idx < n && chars[idx] >= '0' && chars[idx] <= '9') {
                u = u * 10 + (chars[idx] - '0');
                idx++;
            }
            
            // Skip ,"s":
            idx += 4;
            
            // Parse s
            long s = 0;
            while (idx < n && chars[idx] >= '0' && chars[idx] <= '9') {
                s = s * 10 + (chars[idx] - '0');
                idx++;
            }
            
            // Skip ,"f":
            idx += 4;
            
            // Parse f
            boolean f = false;
            if (chars[idx] == 't') {
                f = true;
                idx += 4;
            } else {
                idx += 5;
            }
            
            // The rest of the string is ,"tag":"x\d+"}
            // We don't need to parse the tag content for the checksum, just ensure we consume it if necessary.
            // However, the task only requires the checksum based on u, s, and f.
            
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}