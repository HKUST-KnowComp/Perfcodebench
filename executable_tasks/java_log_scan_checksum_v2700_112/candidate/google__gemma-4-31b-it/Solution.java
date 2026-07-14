import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int pos = 0;

            // Expect "src=S"
            if (len < 5 || row.startsWith("src=S", pos)) {
                pos += 5;
            } else continue;

            // Parse src
            int src = 0;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                src = src * 10 + (row.charAt(pos++) - '0');
            }

            // Expect " lat="
            if (pos + 5 <= len && row.startsWith(" lat=", pos)) {
                pos += 5;
            } else continue;

            // Parse lat
            int lat = 0;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                lat = lat * 10 + (row.charAt(pos++) - '0');
            }

            // Expect " code="
            if (pos + 6 <= len && row.startsWith(" code=", pos)) {
                pos += 6;
            } else continue;

            // Parse code
            int code = 0;
            while (pos < len && row.charAt(pos) >= '0' && row.charAt(pos) <= '9') {
                code = code * 10 + (row.charAt(pos++) - '0');
            }

            // Expect " ok="
            if (pos + 4 <= len && row.startsWith(" ok=", pos)) {
                pos += 4;
            } else continue;

            // Parse ok (yes/no)
            boolean ok = false;
            if (pos + 3 <= len && row.charAt(pos) == 'y' && row.charAt(pos+1) == 'e' && row.charAt(pos+2) == 's') {
                ok = true;
                pos += 3;
            } else if (pos + 2 <= len && row.charAt(pos) == 'n' && row.charAt(pos+1) == 'o') {
                ok = false;
                pos += 2;
            } else continue;

            // Ensure we reached the end of the string to match regex behavior
            if (pos != len) continue;

            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}