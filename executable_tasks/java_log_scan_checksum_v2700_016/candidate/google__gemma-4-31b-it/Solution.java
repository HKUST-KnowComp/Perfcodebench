import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int pos = 0;

            // Expect "src="
            pos += 4; 
            int src = 0;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                src = src * 10 + (row.charAt(pos) - '0');
                pos++;
            }

            // Expect " lat="
            pos += 5;
            int lat = 0;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                lat = lat * 10 + (row.charAt(pos) - '0');
                pos++;
            }

            // Expect " code="
            pos += 6;
            int code = 0;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                code = code * 10 + (row.charAt(pos) - '0');
                pos++;
            }

            // Expect " ok="
            pos += 4;
            boolean ok = (row.charAt(pos) == 'y');

            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}