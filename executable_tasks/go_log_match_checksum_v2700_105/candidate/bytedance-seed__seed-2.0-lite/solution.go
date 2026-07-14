package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	fnvPrime := uint64(1099511628211)
	for _, row := range rows {
		var v uint64 = 0
		if len(row) == 15 {
			valid := true
			// Validate first two uppercase letters
			if row[0] < 'A' || row[0] > 'Z' { valid = false }
			if valid && (row[1] < 'A' || row[1] > 'Z') { valid = false }
			// Validate four digits at positions 2-5
			if valid && (row[2] < '0' || row[2] > '9') { valid = false }
			if valid && (row[3] < '0' || row[3] > '9') { valid = false }
			if valid && (row[4] < '0' || row[4] > '9') { valid = false }
			if valid && (row[5] < '0' || row[5] > '9') { valid = false }
			// Validate hyphen separator
			if valid && row[6] != '-' { valid = false }
			// Validate 8 hex characters at positions 7-14
			for i := 7; i <= 14 && valid; i++ {
				c := row[i]
				if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
					valid = false
				}
			}
			if valid {
				v = uint64(row[2] + row[7])
			}
		}
		// Preserve original exact hash update logic
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}