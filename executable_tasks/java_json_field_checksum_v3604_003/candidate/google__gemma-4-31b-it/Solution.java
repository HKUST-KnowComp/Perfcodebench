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
            while (i < len && row.charAt(i) != 'u') i++;
            i += 3; // skip u":
            while (i < len && Character.isDigit(row.charAt(i))) {
                u = u * 10 + (row.charAt(i++) - '0');
            }

            // Skip to "s":
            while (i < len && row.charAt(i) != 's') i++;
            i += 3; // skip s":
            while (i < len && Character.isDigit(row.charAt(i))) {
                s = s * 10 + (row.charAt(i++) - '0');
            }

            // Skip to "f":
            while (i < len && row.charAt(i) != 'f') i++;
            i += 3; // skip f":
            if (i < len && row.charAt(i) == 't') {
                f = true;
            }

            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}