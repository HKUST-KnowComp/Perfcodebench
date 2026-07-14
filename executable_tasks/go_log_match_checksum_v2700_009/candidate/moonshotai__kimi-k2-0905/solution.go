package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime  = 1099511628211

	hash := fnvOffset
	for _, row := range rows {
		if len(row) != 15 {
			hash *= fnvPrime
			continue
		}

		// manually validate ^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$
		b := *(*[]byte)(unsafe.Pointer(&row)) // zero-allocation cast to []byte

		ok := (b[0]-'A') < 26 && (b[1]-'A') < 26 &&
			(b[2]-'0') < 10 && (b[3]-'0') < 10 && (b[4]-'0') < 10 && (b[5]-'0') < 10 &&
			b[6] == '-' &&
			((b[7]>>4) == 0x3) && // 0-9 or A-F
			((b[8]>>4) == 0x3) &&
			((b[9]>>4) == 0x3) &&
			((b[10]>>4) == 0x3) &&
			((b[11]>>4) == 0x3) &&
			((b[12]>>4) == 0x3) &&
			((b[13]>>4) == 0x3) &&
			((b[14]>>4) == 0x3)

		v := uint64(0)
		if ok {
			v = uint64(b[2] + b[7])
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}

import "unsafe"