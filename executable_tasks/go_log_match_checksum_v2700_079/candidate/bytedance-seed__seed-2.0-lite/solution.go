package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		// Exact match check identical to original regex, zero regex overhead
		if len(row) == 15 {
			// Early exit for missing fixed hyphen to avoid further checks
			if row[6] == '-' {
				// Validate first two uppercase A-Z characters
				ok := (row[0] >= 'A' && row[0] <= 'Z') && (row[1] >= 'A' && row[1] <= 'Z')
				// Validate next four numeric digits
				ok = ok && (row[2] >= '0' && row[2] <= '9') && (row[3] >= '0' && row[3] <= '9') && (row[4] >= '0' && row[4] <= '9') && (row[5] >= '0' && row[5] <= '9')
				// Validate all 8 uppercase hex characters after hyphen
				ok = ok && (row[7] >= '0' && row[7] <= '9' || row[7] >= 'A' && row[7] <= 'F')
				ok = ok && (row[8] >= '0' && row[8] <= '9' || row[8] >= 'A' && row[8] <= 'F')
				ok = ok && (row[9] >= '0' && row[9] <= '9' || row[9] >= 'A' && row[9] <= 'F')
				ok = ok && (row[10] >= '0' && row[10] <= '9' || row[10] >= 'A' && row[10] <= 'F')
				ok = ok && (row[11] >= '0' && row[11] <= '9' || row[11] >= 'A' && row[11] <= 'F')
				ok = ok && (row[12] >= '0' && row[12] <= '9' || row[12] >= 'A' && row[12] <= 'F')
				ok = ok && (row[13] >= '0' && row[13] <= '9' || row[13] >= 'A' && row[13] <= 'F')
				ok = ok && (row[14] >= '0' && row[14] <= '9' || row[14] >= 'A' && row[14] <= 'F')
				if ok {
					v = uint64(row[2] + row[7])
				}
			}
		}
		// Preserve original hash calculation logic exactly
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}