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
	n := len(ids)
	buf := make([]byte, n*16) // 4+4+2+2 = 12, but we write 16 for alignment
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		off := 0
		for i := 0; i < n; i++ {
			*(*uint32)(unsafe.Pointer(&buf[off])) = ids[i]
			off += 4
			*(*uint32)(unsafe.Pointer(&buf[off])) = ts[i]
			off += 4
			*(*uint16)(unsafe.Pointer(&buf[off])) = levels[i]
			off += 2
			*(*uint16)(unsafe.Pointer(&buf[off])) = lens[i]
			off += 2
			// pad to 16 bytes
			off += 4
		}
		hash = solutionChecksumBytes(buf[:off])
	}
	return hash
}