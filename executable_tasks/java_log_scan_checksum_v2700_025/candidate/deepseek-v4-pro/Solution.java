import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int pos = 5; // skip "src=S"

            // parse src
            int src = 0;
            while (pos < len && row.charAt(pos) != ' ') {
                src = src * 10 + (row.charAt(pos) - '0');
                pos++;
            }
            pos += 5; // skip " lat="

            // parse lat
            int lat = 0;
            while (pos < len && row.charAt(pos) != ' ') {
                lat = lat * 10 + (row.charAt(pos) - '0');
                pos++;
            }
            pos += 6; // skip " code="

            // parse code
            int code = 0;
            while (pos < len && row.charAt(pos) != ' ') {
                code = code * 10 + (row.charAt(pos) - '0');
                pos++;
            }
            pos += 4; // skip " ok="

            // ok field
            boolean ok = row.charAt(pos) == 'y';

            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}