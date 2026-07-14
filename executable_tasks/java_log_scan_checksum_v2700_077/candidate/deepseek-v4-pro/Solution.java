import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int idx = 5; // after "src=S"
            int src = 0;
            char c;
            while (idx < len && (c = row.charAt(idx)) != ' ') {
                src = src * 10 + (c - '0');
                idx++;
            }
            idx += 5;
            int lat = 0;
            while (idx < len && (c = row.charAt(idx)) != ' ') {
                lat = lat * 10 + (c - '0');
                idx++;
            }
            idx += 6;
            int code = 0;
            while (idx < len && (c = row.charAt(idx)) != ' ') {
                code = code * 10 + (c - '0');
                idx++;
            }
            idx += 4;
            boolean ok = row.charAt(idx) == 'y';
            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}