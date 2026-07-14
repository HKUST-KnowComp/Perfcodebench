package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime  = 1099511628211
	hash := fnvOffset

	for _, row := range rows {
		// fast length check
		if len(row) != 14 {
			hash ^= 1
			hash *= fnvPrime
			continue
		}

		// manual pattern match: AA0000-00000000
		// positions 0,1: [A-Z]
		// positions 2,3,4,5: [0-9]
		// position 6: '-'
		// positions 7,8,9,10,11,12,13: [0-9A-F]
		ok := true
		ok = ok && row[0] >= 'A' && row[0] <= 'Z'
		ok = ok && row[1] >= 'A' && row[1] <= 'Z'
		ok = ok && row[2] >= '0' && row[2] <= '9'
		ok = ok && row[3] >= '0' && row[3] <= '9'
		ok = ok && row[4] >= '0' && row[4] <= '9'
		ok = ok && row[5] >= '0' && row[5] <= '9'
		ok = ok && row[6] == '-'
		ok = ok && ((row[7]  >= '0' && row[7]  <= '9') || (row[7]  >= 'A' && row[7]  <= 'F'))
		ok = ok && ((row[8]  >= '0' && row[8]  <= '9') || (row[8]  >= 'A' && row[8]  <= 'F'))
		ok = ok && ((row[9]  >= '0' && row[9]  <= '9') || (row[9]  >= 'A' && row[9]  <= 'F'))
		ok = ok && ((row[10] >= '0' && row[10] <= '9') || (row[10] >= 'A' && row[10] <= 'F'))
		ok = ok && ((row[11] >= '0' && row[11] <= '9') || (row[11] >= 'A' && row[11] <= 'F'))
		ok = ok && ((row[12] >= '0' && row[12] <= '9') || (row[12] >= 'A' && row[12] <= 'F'))
		ok = ok && ((row[13] >= '0' && row[13] <= '9') || (row[13] >= 'A' && row[13] <= 'F'))

		v := uint64(0)
		if ok {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}