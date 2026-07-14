package main

func run(rows []string) uint64 {
	const (
		prime uint64 = 1099511628211
		seed  uint64 = 1469598103934665603
	)
	hash := seed
	for _, row := range rows {
		// Fast length check first.
		if len(row) != 15 {
			hash ^= 1
			hash *= prime
			continue
		}

		// Manually validate the fixed pattern:
		// positions: 0-1 upper A-Z, 2-5 digits, 6 dash, 7-14 hex digits.
		ok := true
		for i := 0; i < 2; i++ {
			c := row[i]
			if c < 'A' || c > 'Z' {
				ok = false
				break
			}
		}
		if ok {
			for i := 2; i < 6; i++ {
				c := row[i]
				if c < '0' || c > '9' {
					ok = false
					break
				}
			}
		}
		if ok && row[6] != '-' {
			ok = false
		}
		if ok {
			for i := 7; i < 15; i++ {
				c := row[i]
				if !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
					ok = false
					break
				}
			}
		}

		var v uint64
		if ok {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}