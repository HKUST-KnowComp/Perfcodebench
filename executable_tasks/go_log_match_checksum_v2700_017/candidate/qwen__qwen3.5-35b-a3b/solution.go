package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		if len(row) != 15 {
			continue
		}
		if row[0] < 'A' || row[0] > 'Z' || row[1] < 'A' || row[1] > 'Z' {
			continue
		}
		valid := true
		for i := 2; i < 6; i++ {
			if row[i] < '0' || row[i] > '9' {
				valid = false
				break
			}
		}
		if !valid {
			continue
		}
		if row[6] != '-' {
			continue
		}
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
		v := uint64(row[2] + row[7])
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}