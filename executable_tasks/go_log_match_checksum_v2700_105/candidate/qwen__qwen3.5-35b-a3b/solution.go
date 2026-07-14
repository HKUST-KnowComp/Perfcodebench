package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		if len(row) != 15 {
			continue
		}

		// Check first two uppercase letters
		if row[0] < 'A' || row[0] > 'Z' || row[1] < 'A' || row[1] > 'Z' {
			continue
		}

		// Check next four digits
		if row[2] < '0' || row[2] > '9' ||
		   row[3] < '0' || row[3] > '9' ||
		   row[4] < '0' || row[4] > '9' ||
		   row[5] < '0' || row[5] > '9' {
			continue
		}

		// Check hyphen
		if row[6] != '-' {
			continue
		}

		// Check last eight hex digits
		valid := true
		for i := 7; i < 15; i++ {
			c := row[i]
			if (c < '0' || c > '9') && (c < 'A' || c > 'F') {
				valid = false
				break
			}
		}
		if !valid {
			continue
		}

		var v uint64 = uint64(row[2] + row[7])
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}