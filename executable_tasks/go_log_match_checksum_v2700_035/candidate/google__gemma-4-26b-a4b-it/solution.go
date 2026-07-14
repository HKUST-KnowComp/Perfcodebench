package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var v uint64 = 0
		if len(row) == 13 {
			// Pattern: ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
			// Index:   01 2 3456 7 89ABC
			match := true
			
			// [A-Z]{2}
			if !(row[0] >= 'A' && row[0] <= 'Z') || !(row[1] >= 'A' && row[1] <= 'Z') {
				match = false
			} else if !(row[2] >= '0' && row[2] <= '9') || !(row[3] >= '0' && row[3] <= '9') ||
				!(row[4] >= '0' && row[4] <= '9') || !(row[5] >= '0' && row[5] <= '9') {
				match = false
			} else if row[6] != '-' {
				match = false
			} else {
				// [0-9A-F]{8}
				for i := 7; i < 13; i++ {
					c := row[i]
					if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
						match = false
						break
					}
				}
			}

			if match {
				v = uint64(row[2] + row[7])
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}