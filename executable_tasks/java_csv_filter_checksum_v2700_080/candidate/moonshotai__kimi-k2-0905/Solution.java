import java.util.*;

public class Solution {
    private static final long FNV64_OFFSET = 1469598103934665603L;
    private static final long FNV64_PRIME  = 1099511628211L;

    public static long run(List<String> rows) {
        long hash = FNV64_OFFSET;
        for (String row : rows) {
            int b = 0, c = 0;
            boolean hot = false;

            byte[] buf = row.getBytes();
            int len = buf.length;
            int idx = 0;

            // field 0
            while (idx < len && buf[idx++] != ',') { }
            // field 1
            while (idx < len && buf[idx++] != ',') { }

            // field 2: b
            int sign = 1;
            if (buf[idx] == '-') { sign = -1; idx++; }
            while (idx < len && buf[idx] != ',') {
                b = b * 10 + (buf[idx++] - '0');
            }
            b *= sign;
            idx++; // skip comma

            // field 3: c
            sign = 1;
            if (buf[idx] == '-') { sign = -1; idx++; }
            while (idx < len && buf[idx] != ',') {
                c = c * 10 + (buf[idx++] - '0');
            }
            c *= sign;
            idx++; // skip comma

            // field 4: hot flag (last field)
            hot = (len - idx == 3 &&
                   buf[idx]     == 'h' &&
                   buf[idx + 1] == 'o' &&
                   buf[idx + 2] == 't');

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= FNV64_PRIME;
        }
        return hash;
    }
}