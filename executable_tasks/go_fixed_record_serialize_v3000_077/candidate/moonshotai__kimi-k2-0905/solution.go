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
	buf := make([]byte, n*12)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		// Serialize all records into buf
		for i := 0; i < n; i++ {
			off := i * 12

			// ids[i] -> 4 bytes
			*(*uint32)(unsafe.Pointer(&buf[off])) = ids[i]

			// ts[i] -> 4 bytes
			*(*uint32)(unsafe.Pointer(&buf[off+4])) = ts[i]

			// levels[i], lens[i] -> 2 + 2 bytes
			*(*uint16)(unsafe.Pointer(&buf[off+8])) = levels[i]
			*(*uint16)(unsafe.Pointer(&buf[off+10])) = lens[i]
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}