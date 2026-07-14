import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int src = 0, lat = 0, code = 0;
            boolean ok;
            int i = 5; // skip "src=S"
            char c;
            while ((c = row.charAt(i)) != ' ') {
                src = src * 10 + (c - '0');
                i++;
            }
            i += 5; // skip " lat="
            while ((c = row.charAt(i)) != ' ') {
                lat = lat * 10 + (c - '0');
                i++;
            }
            i += 6; // skip " code="
            while ((c = row.charAt(i)) != ' ') {
                code = code * 10 + (c - '0');
                i++;
            }
            i += 4; // skip " ok="
            ok = row.charAt(i) == 'y';
            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}