import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int idx = 5; // skip "src=S"
            // parse src digits
            int srcStart = idx;
            while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') idx++;
            int src = parseDigits(row, srcStart, idx);
            idx++; // skip space
            // parse lat: skip "lat="
            idx += 4;
            int latStart = idx;
            while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') idx++;
            int lat = parseDigits(row, latStart, idx);
            idx++; // skip space
            // parse code: skip "code="
            idx += 5;
            int codeStart = idx;
            while (idx < len && row.charAt(idx) >= '0' && row.charAt(idx) <= '9') idx++;
            int code = parseDigits(row, codeStart, idx);
            idx++; // skip space
            // parse ok: skip "ok="
            idx += 3;
            boolean ok = (row.charAt(idx) == 'y');
            // compute hash update
            int v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }

    private static int parseDigits(String s, int start, int end) {
        int val = 0;
        for (int i = start; i < end; i++) {
            val = val * 10 + (s.charAt(i) - '0');
        }
        return val;
    }
}