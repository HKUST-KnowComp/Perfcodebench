import java.util.*;

public class Solution {

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            char[] chars = row.toCharArray();
            int n = chars.length;
            
            // Expected format: {"u":(\d+),"s":(\d+),"f":(true|false),"tag":"x\d+"}
            // We use a manual scan to avoid Regex overhead.
            
            int idx = 0;
            // Skip {"u":
            if (n < 7 || chars[0] != '{' || chars[1] != '"' || chars[2] != 'u' || chars[3] != '"' || chars[4] != ':' ) continue;
            idx = 5;
            
            // Parse u
            long u = 0;
            while (idx < n && chars[idx] >= '0' && chars[idx] <= '9') {
                u = u * 10 + (chars[idx] - '0');
                idx++;
            }
            
            // Skip ,"s":
            if (idx + 5 >= n || chars[idx] != ',' || chars[idx+1] != '"' || chars[idx+2] != 's' || chars[idx+3] != '"' || chars[idx+4] != ':') continue;
            idx += 5;
            
            // Parse s
            long s = 0;
            while (idx < n && chars[idx] >= '0' && chars[idx] <= '9') {
                s = s * 10 + (chars[idx] - '0');
                idx++;
            }
            
            // Skip ,"f":
            if (idx + 5 >= n || chars[idx] != ',' || chars[idx+1] != '"' || chars[idx+2] != 'f' || chars[idx+3] != '"' || chars[idx+4] != ':') continue;
            idx += 5;
            
            // Parse f
            boolean f = false;
            if (idx + 4 <= n && chars[idx] == 't' && chars[idx+1] == 'r' && chars[idx+2] == 'u' && chars[idx+3] == 'e') {
                f = true;
                idx += 4;
            } else if (idx + 5 <= n && chars[idx] == 'f' && chars[idx+1] == 'a' && chars[idx+2] == 'l' && chars[idx+3] == 's' && chars[idx+4] == 'e') {
                f = false;
                idx += 5;
            } else {
                continue;
            }
            
            // Skip ,"tag":"x\d+"}
            // The regex requires "tag":"x\d+"}
            // We just need to ensure the structure is valid enough to match the original logic
            // The original regex was: ^\{\"u\":(\d+),\"s\":(\d+),\"f\":(true|false),\"tag\":\"x\d+\"\}$
            
            // Check for ,"tag":"x
            if (idx + 8 >= n || chars[idx] != ',' || chars[idx+1] != '"' || chars[idx+2] != 't' || chars[idx+3] != 'a' || chars[idx+4] != 'g' || chars[idx+5] != '"' || chars[idx+6] != ':' || chars[idx+7] != '"' || chars[idx+8] != 'x') continue;
            idx += 9;
            
            // Skip digits in tag
            while (idx < n && chars[idx] >= '0' && chars[idx] <= '9') {
                idx++;
            }
            
            // Check for "}
            if (idx + 2 != n || chars[idx] != '"' || chars[idx+1] != '}') continue;

            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}