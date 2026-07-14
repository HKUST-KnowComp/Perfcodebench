import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            char[] ch = row.toCharArray();
            int pos = 4; // skip "src=S"

            // parse src (digits after 'S')
            int src = 0;
            while (ch[pos] >= '0' && ch[pos] <= '9') {
                src = src * 10 + (ch[pos++] - '0');
            }
            pos++; // skip space

            // skip "lat="
            pos += 4;

            // parse lat
            int lat = 0;
            while (ch[pos] >= '0' && ch[pos] <= '9') {
                lat = lat * 10 + (ch[pos++] - '0');
            }
            pos++; // skip space

            // skip "code="
            pos += 5;

            // parse code
            int code = 0;
            while (ch[pos] >= '0' && ch[pos] <= '9') {
                code = code * 10 + (ch[pos++] - '0');
            }
            pos++; // skip space

            // skip "ok="
            pos += 3;

            // check "yes"
            boolean ok = (ch[pos] == 'y' && ch[pos + 1] == 'e' && ch[pos + 2] == 's');

            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}