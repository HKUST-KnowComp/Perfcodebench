package main

func run(rows []string) uint64 {
	const prime = 1099511628211
	hash := uint64(1469598103934665603)

	for _, row := range rows {
		if len(row) != 13 {
			continue
		}

		// fast path: check format manually
		ok := true
		ok = ok && row[0] >= 'A' && row[0] <= 'Z'
		ok = ok && row[1] >= 'A' && row[1] <= 'Z'
		ok = ok && row[2] >= '0' && row[2] <= '9'
		ok = ok && row[3] >= '0' && row[3] <= '9'
		ok = ok && row[4] >= '0' && row[4] <= '9'
		ok = ok && row[5] >= '0' && row[5] <= '9'
		ok = ok && row[6] == '-'
		ok = ok && ((row[7] >= '0' && row[7] <= '9') || (row[7] >= 'A' && row[7] <= 'F'))
		ok = ok && ((row[8] >= '0' && row[8] <= '9') || (row[8] >= 'A' && row[8] <= 'F'))
		ok = ok && ((row[9] >= '0' && row[9] <= '9') || (row[9] >= 'A' && row[9] <= 'F'))
		ok = ok && ((row[10] >= '0' && row[10] <= '9') || (row[10] >= 'A' && row[10] <= 'F'))
		ok = ok && ((row[11] >= '0' && row[11] <= '9') || (row[11] >= 'A' && row[11] <= 'F'))
		ok = ok && ((row[12] >= '0' && row[12] <= '9') || (row[12] >= 'A' && row[12] <= 'F'))

		if !ok {
			continue
		}

		v := uint64(row[2] + row[7])
		hash ^= v + 1
		hash *= prime
	}
	return hash
}