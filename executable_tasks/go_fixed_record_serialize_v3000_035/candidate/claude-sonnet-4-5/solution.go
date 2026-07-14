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
	recordSize := 12
	n := len(ids)
	bufSize := n * recordSize
	out := make([]byte, bufSize)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		pos := 0
		for i := 0; i < n; i++ {
			id := ids[i]
			*(*uint32)(unsafe.Pointer(&out[pos])) = id
			pos += 4
			
			t := ts[i]
			*(*uint32)(unsafe.Pointer(&out[pos])) = t
			pos += 4
			
			lvl := levels[i]
			*(*uint16)(unsafe.Pointer(&out[pos])) = lvl
			pos += 2
			
			ln := lens[i]
			*(*uint16)(unsafe.Pointer(&out[pos])) = ln
			pos += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}