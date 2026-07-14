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
	recSize := 12 // 4+4+2+2
	buf := make([]byte, n*recSize)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		// reset buffer
		_ = buf[:len(buf)] // bounds hint
		// write records
		dst := buf
		for i := 0; i < n; i++ {
			*(*uint32)(unsafe.Pointer(&dst[0])) = ids[i]
			*(*uint32)(unsafe.Pointer(&dst[4])) = ts[i]
			*(*uint16)(unsafe.Pointer(&dst[8])) = levels[i]
			*(*uint16)(unsafe.Pointer(&dst[10])) = lens[i]
			dst = dst[recSize:]
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}