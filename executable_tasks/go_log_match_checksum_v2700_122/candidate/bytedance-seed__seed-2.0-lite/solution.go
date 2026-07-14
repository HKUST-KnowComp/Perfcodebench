package main

func run(rows []string) uint64 {
	const fnvOffset64 = 1469598103934665603
	const fnvPrime64 = 1099511628211
	hash := uint64(fnvOffset64)
	for _, row := range rows {
		var v uint64 = 0
		// Fail fast for strings that can't match the fixed-length pattern first
		if len(row) == 15 {
			// Check mandatory hyphen at fixed position 6, another cheap early exit
			if row[6] == '-' {
				// Validate first two characters are uppercase A-Z
				c0, c1 := row[0], row[1]
				if c0 >= 'A' && c0 <= 'Z' && c1 >= 'A' && c1 <= 'Z' {
					// Validate 4 numeric digits at positions 2-5
					c2, c3, c4, c5 := row[2], row[3], row[4], row[5]
					if c2 >= '0' && c2 <= '9' && c3 >= '0' && c3 <= '9' && c4 >= '0' && c4 <= '9' && c5 >= '0' && c5 <= '9' {
						// Validate 8 uppercase hex characters at positions 7-14
						validHex := true
						for i := 7; i <= 14; i++ {
							c := row[i]
							if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
								validHex = false
								break
							}
						}
						if validHex {
							// Exact same value calculation as original regex-matching path
							v = uint64(row[2] + row[7])
						}
					}
				}
			}
		}
		// Preserve identical FNV hash logic to maintain correct checksum values
		hash ^= v + 1
		hash *= fnvPrime64
	}
	return hash
}