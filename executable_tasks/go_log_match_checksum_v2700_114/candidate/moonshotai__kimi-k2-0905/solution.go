package main

func run(rows []string) uint64 {
	const (
		prime1 = 1469598103934665603
		prime2 = 1099511628211
	)
	hash := prime1

	for _, row := range rows {
		if len(row) != 14 {
			hash ^= 1
			hash *= prime2
			continue
		}

		b := *(*[]byte)(unsafe.Pointer(&row))

		ok := (b[0]&0xDF) >= 'A' && (b[0]&0xDF) <= 'Z' &&
			(b[1]&0xDF) >= 'A' && (b[1]&0xDF) <= 'Z' &&
			b[2] >= '0' && b[2] <= '9' &&
			b[3] >= '0' && b[3] <= '9' &&
			b[4] >= '0' && b[4] <= '9' &&
			b[5] >= '0' && b[5] <= '9' &&
			b[6] == '-' &&
			isHex(b[7]) && isHex(b[8]) && isHex(b[9]) && isHex(b[10]) &&
			isHex(b[11]) && isHex(b[12]) && isHex(b[13]) && isHex(b[14])

		var v uint64
		if ok {
			v = uint64(b[2]) + uint64(b[7])
		}

		hash ^= v + 1
		hash *= prime2
	}
	return hash
}

func isHex(c byte) bool {
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')
}

import "unsafe"