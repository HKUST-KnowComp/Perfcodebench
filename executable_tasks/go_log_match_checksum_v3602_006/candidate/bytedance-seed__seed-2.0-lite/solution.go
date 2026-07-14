package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		// Exact length of the required pattern: 2 uppercase +4 digits +1 hyphen +8 hex = 15 bytes
		if len(row) == 15 {
			// Validate first two characters are A-Z
			if (row[0] >= 'A' && row[0] <= 'Z') && (row[1] >= 'A' && row[1] <= 'Z') {
				// Validate next four characters are 0-9
				if (row[2] >= '0' && row[2] <= '9') && (row[3] >= '0' && row[3] <= '9') &&
					(row[4] >= '0' && row[4] <= '9') && (row[5] >= '0' && row[5] <= '9') {
					// Validate hyphen separator
					if row[6] == '-' {
						// Validate all 8 trailing hex characters are 0-9 or A-F
						validHex := true
						for i := 7; i < 15; i++ {
							c := row[i]
							if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
								validHex = false
								break
							}
						}
						if validHex {
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