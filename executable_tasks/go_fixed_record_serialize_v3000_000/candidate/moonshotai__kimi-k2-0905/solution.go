package main

import (
	"unsafe"
)

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	// 10 bytes per record: 4 + 4 + 2 + 2
	n := len(ids)
	buf := make([]byte, 10*n)

	// Pre-compute starting pointers to avoid bounds checks in the inner loop.
	idPtr := (*[4]byte)(unsafe.Pointer(&ids[0]))
	tsPtr := (*[4]byte)(unsafe.Pointer(&ts[0]))
	lvlPtr := (*[2]byte)(unsafe.Pointer(&levels[0]))
	lenPtr := (*[2]byte)(unsafe.Pointer(&lens[0]))

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		b := buf
		for i := 0; i < n; i++ {
			// id: little-endian 4 bytes
			*(*uint32)(unsafe.Pointer(&b[0])) = ids[i]
			// ts: little-endian 4 bytes
			*(*uint32)(unsafe.Pointer(&b[4])) = ts[i]
			// level: little-endian 2 bytes
			*(*uint16)(unsafe.Pointer(&b[8])) = levels[i]
			// len: little-endian 2 bytes
			*(*uint16)(unsafe.Pointer(&b[10])) = lens[i]
			b = b[12:] // advance 12 bytes (4+4+2+2)
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}