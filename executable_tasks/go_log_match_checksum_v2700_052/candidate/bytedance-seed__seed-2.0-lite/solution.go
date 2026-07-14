package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		// Fast manual validation matching original regex, short-circuits early for invalid rows
		if len(row) == 15 && row[6] == '-' {
			// Validate first two are uppercase A-Z
			if row[0] >= 'A' && row[0] <= 'Z' && row[1] >= 'A' && row[1] <= 'Z' {
				// Validate next four are digits 0-9
				if row[2] >= '0' && row[2] <= '9' && row[3] >= '0' && row[3] <= '9' &&
					row[4] >= '0' && row[4] <= '9' && row[5] >= '0' && row[5] <= '9' {
					// Validate final 8 are 0-9A-F hex characters
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
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}