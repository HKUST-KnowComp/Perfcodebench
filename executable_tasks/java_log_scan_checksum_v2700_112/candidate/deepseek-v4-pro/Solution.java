import java.util.*;

public class Solution {
    private static final long XOR_INIT = 1469598103934665603L;
    private static final long MULT = 1099511628211L;

    public static long run(List<String> rows) {
        long hash = XOR_INIT;
        for (String s : rows) {
            int len = s.length();
            int pos = 5; // skip "src=S"
            // parse src
            int src = 0;
            while (pos < len) {
                char c = s.charAt(pos);
                if (c == ' ') break;
                src = src * 10 + (c - '0');
                pos++;
            }
            // skip space + "lat="
            pos += 5;
            // parse lat
            int lat = 0;
            while (pos < len) {
                char c = s.charAt(pos);
                if (c == ' ') break;
                lat = lat * 10 + (c - '0');
                pos++;
            }
            // skip space + "code="
            pos += 6;
            // parse code
            int code = 0;
            while (pos < len) {
                char c = s.charAt(pos);
                if (c == ' ') break;
                code = code * 10 + (c - '0');
                pos++;
            }
            // skip space + "ok="
            pos += 4;
            // check yes/no
            boolean ok = (s.charAt(pos) == 'y');
            
            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= MULT;
        }
        return hash;
    }
}
