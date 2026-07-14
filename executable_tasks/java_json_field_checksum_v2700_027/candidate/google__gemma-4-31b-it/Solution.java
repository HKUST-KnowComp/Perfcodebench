import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int u = 0, s = 0;
            boolean f = false;
            int len = row.length();
            int pos = 0;

            // Expected format: {"u":(digits),"s":(digits),"f":(true|false),"tag":"x(digits)"}
            // Skip to "u":
            pos += 5; // {"u":
            while (pos < len && row.charAt(pos) != ',') {
                u = u * 10 + (row.charAt(pos++) - '0');
            }
            
            // Skip to "s":
            pos += 5; // ,"s":
            while (pos < len && row.charAt(pos) != ',') {
                s = s * 10 + (row.charAt(pos++) - '0');
            }

            // Skip to "f":
            pos += 5; // ,"f":
            if (row.charAt(pos) == 't') {
                f = true;
                pos += 4; // true
            } else {
                f = false;
                pos += 5; // false
            }

            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}