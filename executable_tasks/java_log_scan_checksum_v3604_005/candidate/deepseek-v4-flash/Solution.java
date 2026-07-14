import java.util.*;

public class Solution {
    private static int parseInt(String s, int start, int end) {
        int result = 0;
        while (start < end) {
            result = result * 10 + (s.charAt(start) - '0');
            start++;
        }
        return result;
    }

    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final long PRIME = 1099511628211L;

        for (String s : rows) {
            // positions determined by fixed format: src=S<digits> lat=<digits> code=<digits> ok=yes|no
            // 0..4 : "src=S"  5.. : src digits until space
            int pos = 5;
            int srcStart = pos;
            while (s.charAt(pos) != ' ') pos++;
            int src = parseInt(s, srcStart, pos);

            // skip space and "lat=" (1+4)
            pos += 5;
            int latStart = pos;
            while (s.charAt(pos) != ' ') pos++;
            int lat = parseInt(s, latStart, pos);

            // skip space and "code=" (1+5)
            pos += 6;
            int codeStart = pos;
            while (s.charAt(pos) != ' ') pos++;
            int code = parseInt(s, codeStart, pos);

            // skip space and "ok=" (1+3)
            pos += 4;
            boolean ok = s.charAt(pos) == 'y';  // 'y' means yes, otherwise no

            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= PRIME;
        }
        return hash;
    }
}