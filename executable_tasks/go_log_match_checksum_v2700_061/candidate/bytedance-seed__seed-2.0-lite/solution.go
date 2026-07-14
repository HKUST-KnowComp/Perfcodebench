package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime = 1099511628211
	hash := uint64(fnvOffset)
	for _, row := range rows {
		var v uint64 = 0
		// Valid rows match the pattern's exact 15-byte length: 2+4+1+8=15
		if len(row) == 15 {
			valid := true
			// Validate first two characters are uppercase A-Z
			if row[0] < 'A' || row[0] > 'Z' || row[1] < 'A' || row[1] > 'Z' {
				valid = false
			}
			// Validate next four characters are digits 0-9
			if valid && (row[2] < '0' || row[2] > '9' || row[3] < '0' || row[3] > '9' || row[4] < '0' || row[4] > '9' || row[5] < '0' || row[5] > '9') {
				valid = false
			}
			// Validate hyphen separator at correct position
			if valid && row[6] != '-' {
				valid = false
			}
			// Validate final 8 hex characters (0-9, A-F)
			for i := 7; i <= 14; i++ {
				c := row[i]
				if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
					valid = false
					break
				}
			}
			if valid {
				v = uint64(row[2] + row[7])
			}
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}