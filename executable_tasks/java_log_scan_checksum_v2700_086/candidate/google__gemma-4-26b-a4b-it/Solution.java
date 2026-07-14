import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 146959810394665603L; // Corrected constant from baseline if needed, but keeping baseline logic
        // Re-aligning to baseline constant exactly: 1469598103934665603L
        hash = 1469598103934665603L;

        for (int i = 0; i < rows.size(); i++) {
            String row = rows.get(i);
            int len = row.length();
            
            // Manual parsing to avoid Regex and Integer.parseInt overhead
            // Format: ^src=S(\d+) lat=(\d+) code=(\d+) ok=(yes|no)$
            
            int cursor = 4; // skip "src="
            if (cursor >= len || row.charAt(cursor) != 'S') continue;
            cursor++;

            // Parse src
            long src = 0;
            while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
                src = src * 10 + (row.charAt(cursor) - '0');
                cursor++;
            }
            
            // skip " lat=" (5 chars)
            cursor += 5;
            
            // Parse lat
            long lat = 0;
            while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
                lat = lat * 10 + (row.charAt(cursor) - '0');
                cursor++;
            }
            
            // skip " code=" (6 chars)
            cursor += 6;
            
            // Parse code
            long code = 0;
            while (cursor < len && row.charAt(cursor) >= '0' && row.charAt(cursor) <= '9') {
                code = code * 10 + (row.charAt(cursor) - '0');
                cursor++;
            }
            
            // skip " ok=" (4 chars)
            cursor += 4;
            
            // Parse ok
            boolean ok = false;
            if (cursor + 2 < len && row.charAt(cursor) == 'y' && row.charAt(cursor+1) == 'e' && row.charAt(cursor+2) == 's') {
                ok = true;
            }

            long v = (src & 255) + lat + code + (ok ? 31 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}