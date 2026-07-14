import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int src = 0;
            int pos = 5; // skip "src=S"
            char c;
            while (pos < row.length() && (c = row.charAt(pos)) >= '0' && c <= '9') {
                src = src * 10 + (c - '0');
                pos++;
            }
            pos += 5; // skip space + "lat="
            int lat = 0;
            while (pos < row.length() && (c = row.charAt(pos)) >= '0' && c <= '9') {
                lat = lat * 10 + (c - '0');
                pos++;
            }
            pos += 6; // skip space + "code="
            int code = 0;
            while (pos < row.length() && (c = row.charAt(pos)) >= '0' && c <= '9') {
                code = code * 10 + (c - '0');
                pos++;
            }
            pos += 4; // skip space + "ok="
            boolean ok = row.charAt(pos) == 'y';
            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}