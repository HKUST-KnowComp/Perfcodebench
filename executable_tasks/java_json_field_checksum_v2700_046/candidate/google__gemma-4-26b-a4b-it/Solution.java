import java.util.*;

public class Solution {

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            char[] chars = row.toCharArray();
            int n = chars.length;
            
            // Expected format: {"u":123,"s":456,"f":true,"tag":"x123"}
            // We scan for the specific keys to extract values.
            
            int u = 0;
            int s = 0;
            boolean f = false;

            int i = 0;
            // Skip to "u"
            while (i < n && chars[i] != 'u') i++;
            if (i < n) {
                i += 4; // skip ","u":"
                int start = i;
                while (i < n && chars[i] >= '0' && chars[i] <= '9') i++;
                u = parsePositiveInt(chars, start, i);
            }

            // Skip to "s"
            while (i < n && chars[i] != 's') i++;
            if (i < n) {
                i += 4; // skip ","s":"
                int start = i;
                while (i < n && chars[i] >= '0' && chars[i] <= '9') i++;
                s = parsePositiveInt(chars, start, i);
            }

            // Skip to "f"
            while (i < n && chars[i] != 'f') i++;
            if (i < n) {
                i += 4; // skip ","f":"
                if (i < n && chars[i] == 't') {
                    f = true;
                } else {
                    f = false;
                }
            }

            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parsePositiveInt(char[] chars, int start, int end) {
        int res = 0;
        for (int i = start; i < end; i++) {
            res = res * 10 + (chars[i] - '0');
        }
        return res;
    }
}