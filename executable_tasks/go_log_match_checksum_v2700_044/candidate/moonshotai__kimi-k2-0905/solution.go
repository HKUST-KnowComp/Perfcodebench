package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime  = 1099511628211

	hash := fnvOffset
	for _, row := range rows {
		if len(row) != 14 {
			hash ^= 1
			hash *= fnvPrime
			continue
		}

		// [A-Z]{2}
		ok := row[0] >= 'A' && row[0] <= 'Z' &&
		      row[1] >= 'A' && row[1] <= 'Z'
		// [0-9]{4}
		ok = ok && row[2] >= '0' && row[2] <= '9' &&
		          row[3] >= '0' && row[3] <= '9' &&
		          row[4] >= '0' && row[4] <= '9' &&
		          row[5] >= '0' && row[5] <= '9'
		// -
		ok = ok && row[6] == '-'
		// [0-9A-F]{8}
		for i := 7; i < 15; i++ {
			c := row[i]
			ok = ok && ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
		}

		v := uint64(0)
		if ok {
			v = uint64(row[2]) + uint64(row[7])
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}