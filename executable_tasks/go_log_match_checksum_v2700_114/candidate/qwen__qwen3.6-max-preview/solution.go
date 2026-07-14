package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		v := uint64(0)
		if len(row) == 15 {
			r0, r1 := row[0], row[1]
			r2, r3, r4, r5 := row[2], row[3], row[4], row[5]
			r6 := row[6]
			r7, r8, r9, r10, r11, r12, r13, r14 := row[7], row[8], row[9], row[10], row[11], row[12], row[13], row[14]

			if r0 >= 'A' && r0 <= 'Z' && r1 >= 'A' && r1 <= 'Z' &&
				r2 >= '0' && r2 <= '9' && r3 >= '0' && r3 <= '9' && r4 >= '0' && r4 <= '9' && r5 >= '0' && r5 <= '9' &&
				r6 == '-' &&
				isHex(r7) && isHex(r8) && isHex(r9) && isHex(r10) && isHex(r11) && isHex(r12) && isHex(r13) && isHex(r14) {
				v = uint64(r2 + r7)
			}
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func isHex(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}
