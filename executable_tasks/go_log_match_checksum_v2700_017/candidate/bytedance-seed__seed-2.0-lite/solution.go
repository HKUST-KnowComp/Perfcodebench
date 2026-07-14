package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		v := uint64(0)
		// Exact validation equivalent to original regex pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
		if len(row) == 15 {
			// Verify first two characters are uppercase A-Z
			if (row[0] >= 'A' && row[0] <= 'Z') && (row[1] >= 'A' && row[1] <= 'Z') {
				// Verify next four characters are numeric 0-9
				if (row[2] >= '0' && row[2] <= '9') && (row[3] >= '0' && row[3] <= '9') &&
					(row[4] >= '0' && row[4] <= '9') && (row[5] >= '0' && row[5] <= '9') {
					// Verify mandatory separator at position 6
					if row[6] == '-' {
						// Verify final 8 characters are 0-9 or A-F
						validSuffix := true
						for i := 7; i <= 14; i++ {
							c := row[i]
							if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
								validSuffix = false
								break
							}
						}
						if validSuffix {
							v = uint64(row[2] + row[7])
						}
					}
				}
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}