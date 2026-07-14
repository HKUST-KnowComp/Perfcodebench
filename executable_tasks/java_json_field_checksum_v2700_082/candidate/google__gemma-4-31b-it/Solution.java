import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int u = 0, s = 0;
            boolean f = false;
            int len = row.length();
            int i = 0;

            // Expected format: {"u":123,"s":456,"f":true,"tag":"x123"}
            // Skip to "u":
            i += 5; 
            while (i < len && row.charAt(i) != ',') {
                u = u * 10 + (row.charAt(i++) - '0');
            }
            
            // Skip to "s":
            i += 5; 
            while (i < len && row.charAt(i) != ',') {
                s = s * 10 + (row.charAt(i++) - '0');
            }

            // Skip to "f":
            i += 5;
            if (row.charAt(i) == 't') {
                f = true;
            } else {
                f = false;
            }

            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}