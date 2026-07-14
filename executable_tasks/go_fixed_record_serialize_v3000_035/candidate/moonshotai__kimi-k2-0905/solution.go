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
	const recordSize = 12 // 4+4+2+2
	n := len(ids)
	if n == 0 {
		return 0
	}
	total := n * recordSize
	buf := make([]byte, total)
	
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		// Write all records directly into the pre-allocated buffer.
		// Using unsafe avoids per-field bounds checks and shifts.
		ptr := (*[1 << 30]byte)(unsafe.Pointer(&buf[0]))[:total:total]
		off := 0
		for i := 0; i < n; i++ {
			*(*uint32)(unsafe.Pointer(&ptr[off])) = ids[i]
			off += 4
			*(*uint32)(unsafe.Pointer(&ptr[off])) = ts[i]
			off += 4
			*(*uint16)(unsafe.Pointer(&ptr[off])) = levels[i]
			off += 2
			*(*uint16)(unsafe.Pointer(&ptr[off])) = lens[i]
			off += 2
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}