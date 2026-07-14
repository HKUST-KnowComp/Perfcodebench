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
	recordSize := 4 + 4 + 2 + 2 // 12 bytes per record
	buf := make([]byte, n*recordSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		// Serialize all records into buf
		for i := 0; i < n; i++ {
			off := i * recordSize
			// id: 4 bytes little-endian
			*(*uint32)(unsafe.Pointer(&buf[off])) = ids[i]
			// ts: 4 bytes little-endian
			*(*uint32)(unsafe.Pointer(&buf[off+4])) = ts[i]
			// level: 2 bytes little-endian
			*(*uint16)(unsafe.Pointer(&buf[off+8])) = levels[i]
			// len: 2 bytes little-endian
			*(*uint16)(unsafe.Pointer(&buf[off+10])) = lens[i]
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}