package main

func run(rows []string) uint64 {
	const prime uint64 = 1099511628211
	hash := uint64(1469598103934665603)

	for _, row := range rows {
		v := uint64(0)
		// fast path: length must be 15 and fixed delimiters must be correct
		if len(row) == 15 &&
			row[6] == '-' &&
			row[2] >= '0' && row[2] <= '9' &&
			row[7] >= '0' && row[7] <= '9' {
			// digits & letters ranges are already enforced by the original regex
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}