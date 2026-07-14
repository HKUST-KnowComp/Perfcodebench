import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int u = 0, s = 0;
            boolean f = false;
            int len = row.length();
            for (int i = 0; i < len; i++) {
                char c = row.charAt(i);
                if (c == 'u' && row.charAt(i + 1) == '"' && row.charAt(i + 2) == ':') {
                    int p = i + 3;
                    int sign = 1;
                    if (row.charAt(p) == '-') {
                        sign = -1;
                        p++;
                    }
                    while (p < len && row.charAt(p) >= '0' && row.charAt(p) <= '9') {
                        u = u * 10 + (row.charAt(p) - '0');
                        p++;
                    }
                    u *= sign;
                } else if (c == 's' && row.charAt(i + 1) == '"' && row.charAt(i + 2) == ':') {
                    int p = i + 3;
                    int sign = 1;
                    if (row.charAt(p) == '-') {
                        sign = -1;
                        p++;
                    }
                    while (p < len && row.charAt(p) >= '0' && row.charAt(p) <= '9') {
                        s = s * 10 + (row.charAt(p) - '0');
                        p++;
                    }
                    s *= sign;
                } else if (c == 'f' && row.charAt(i + 1) == '"' && row.charAt(i + 2) == ':') {
                    f = row.charAt(i + 3) == 't';
                } else if (c == 'g' && row.charAt(i + 1) == '"' && row.charAt(i + 2) == ':') {
                    break;
                }
            }
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}