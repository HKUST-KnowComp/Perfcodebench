package main

func run(rows []string) uint64 {
	const prime uint64 = 1099511628211
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 {
			valid := true
			// positions 0-1: uppercase letters
			if row[0] < 'A' || row[0] > 'Z' {
				valid = false
			}
			if row[1] < 'A' || row[1] > 'Z' {
				valid = false
			}
			// positions 2-5: digits
			if row[2] < '0' || row[2] > '9' {
				valid = false
			}
			if row[3] < '0' || row[3] > '9' {
				valid = false
			}
			if row[4] < '0' || row[4] > '9' {
				valid = false
			}
			if row[5] < '0' || row[5] > '9' {
				valid = false
			}
			// position 6: hyphen
			if row[6] != '-' {
				valid = false
			}
			// positions 7-14: hex digits (0-9, A-F)
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
		hash *= prime
	}
	return hash
}