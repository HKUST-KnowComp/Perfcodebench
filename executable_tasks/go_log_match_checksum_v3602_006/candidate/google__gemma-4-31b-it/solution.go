package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 {
			valid := true
			// [A-Z]{2}
			if !(row[0] >= 'A' && row[0] <= 'Z' && row[1] >= 'A' && row[1] <= 'Z') {
				valid = false
			} else {
				// [0-9]{4}
				for i := 2; i < 6; i++ {
					if row[i] < '0' || row[i] > '9' {
						valid = false
							break
				}
				}
				if valid && row[6] != '-' {
					valid = false
				}
				if valid {
					// [0-9A-F]{8}
					for i := 7; i < 15; i++ {
						c := row[i]
						if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
							valid = false
							break
					}
					}
				}
			}
			if valid {
				v = uint64(row[2] + row[7])
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}