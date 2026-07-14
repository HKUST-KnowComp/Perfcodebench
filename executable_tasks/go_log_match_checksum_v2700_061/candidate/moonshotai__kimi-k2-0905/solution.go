package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime  = 1099511628211
	hash := fnvOffset

	for _, row := range rows {
		// fast-path: must be exactly 15 bytes
		if len(row) != 15 {
			hash ^= 1
			hash *= fnvPrime
			continue
		}

		r := *(*[15]byte)(*(*uintptr)(&row)) // zero-copy view

		// validate pattern: AA0000-00000000
		// positions 0,1: A-Z
		// positions 2-5: 0-9
		// position 6: '-'
		// positions 7-14: 0-9A-F
		ok := (r[0]-'A') < 26 && (r[1]-'A') < 26 &&
			(r[2]-'0') < 10 && (r[3]-'0') < 10 && (r[4]-'0') < 10 && (r[5]-'0') < 10 &&
			r[6] == '-' &&
			((r[7]>>4) == 0 && (r[7]|0x20)-'a' < 6 || (r[7]-'0') < 10) &&
			((r[8]>>4) == 0 && (r[8]|0x20)-'a' < 6 || (r[8]-'0') < 10) &&
			((r[9]>>4) == 0 && (r[9]|0x20)-'a' < 6 || (r[9]-'0') < 10) &&
			((r[10]>>4) == 0 && (r[10]|0x20)-'a' < 6 || (r[10]-'0') < 10) &&
			((r[11]>>4) == 0 && (r[11]|0x20)-'a' < 6 || (r[11]-'0') < 10) &&
			((r[12]>>4) == 0 && (r[12]|0x20)-'a' < 6 || (r[12]-'0') < 10) &&
			((r[13]>>4) == 0 && (r[13]|0x20)-'a' < 6 || (r[13]-'0') < 10) &&
			((r[14]>>4) == 0 && (r[14]|0x20)-'a' < 6 || (r[14]-'0') < 10)

		v := uint64(0)
		if ok {
			v = uint64(r[2] + r[7])
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}