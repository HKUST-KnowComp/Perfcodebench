package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		if len(row) != 14 {
			hash ^= 1
			hash *= 1099511628211
			continue
		}
		// fast pattern check: AA0000-00000000
		ok := true
		ok = ok && row[0]&0xDF >= 'A' && row[0]&0xDF <= 'Z'
		ok = ok && row[1]&0xDF >= 'A' && row[1]&0xDF <= 'Z'
		ok = ok && row[2] >= '0' && row[2] <= '9'
		ok = ok && row[3] >= '0' && row[3] <= '9'
		ok = ok && row[4] >= '0' && row[4] <= '9'
		ok = ok && row[5] >= '0' && row[5] <= '9'
		ok = ok && row[6] == '-'
		for i := 7; i < 14; i++ {
			c := row[i]
			ok = ok && ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
		}
		v := uint64(0)
		if ok {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}