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
	need := n * 16
	buf := make([]byte, need)
	hash := uint64(1469598103934665603)

	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		off := 0
		for i := 0; i < n; i++ {
			*(*uint64)(unsafe.Pointer(&buf[off])) = *(*uint64)(unsafe.Pointer(&ids[i]))
			*(*uint64)(unsafe.Pointer(&buf[off+8])) = uint64(ts[i]) | (uint64(levels[i]) << 32) | (uint64(lens[i]) << 48)
			off += 16
		}
		for i := 0; i < need; i++ {
			hash ^= uint64(buf[i])
			hash *= 1099511628211
		}
	}
	return hash
}