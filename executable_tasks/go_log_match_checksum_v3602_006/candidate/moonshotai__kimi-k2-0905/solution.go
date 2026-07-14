package main

func run(rows []string) uint64 {
	const fnvOffset = 1469598103934665603
	const fnvPrime  = 1099511628211

	hash := fnvOffset
	for _, row := range rows {
		// fast path: length must be exactly 14
		if len(row) != 14 {
			hash ^= 1
			hash *= fnvPrime
			continue
		}

		r := (*[14]byte)(unsafe.Pointer((*reflect.StringHeader)(unsafe.Pointer(&row)).Data))

		// validate pattern: AA0000-00000000
		// positions 0,1: [A-Z]
		// positions 2,3,4,5: [0-9]
		// position 6: '-'
		// positions 7..13: [0-9A-F]
		ok := true
		ok = ok && r[0] >= 'A' && r[0] <= 'Z'
		ok = ok && r[1] >= 'A' && r[1] <= 'Z'
		ok = ok && r[2] >= '0' && r[2] <= '9'
		ok = ok && r[3] >= '0' && r[3] <= '9'
		ok = ok && r[4] >= '0' && r[4] <= '9'
		ok = ok && r[5] >= '0' && r[5] <= '9'
		ok = ok && r[6] == '-'
		for i := 7; i < 14; i++ {
			c := r[i]
			ok = ok && ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
		}

		v := uint64(0)
		if ok {
			v = uint64(r[2] + r[7])
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}

import (
	"reflect"
	"unsafe"
)