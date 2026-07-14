package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 {
			valid := true
			// first 2 uppercase letters
			for i := 0; i < 2; i++ {
				if row[i] < 'A' || row[i] > 'Z' {
					valid = false
					break
				}
			}
			if valid {
				// next 4 digits
				for i := 2; i < 6; i++ {
					if row[i] < '0' || row[i] > '9' {
						valid = false
						break
					}
				}
			}
			if valid && row[6] == '-' {
				// last 8 hex digits (0-9 or A-F)
				for i := 7; i < 15; i++ {
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
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}