package main

import (
	"unsafe"
)

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	recordSize := 12
	n := len(ids)
	bufSize := n * recordSize
	out := make([]byte, bufSize)
	
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		pos := 0
		
		for i := 0; i < n; i++ {
			*(*uint32)(unsafe.Pointer(&out[pos])) = ids[i]
			pos += 4
			*(*uint32)(unsafe.Pointer(&out[pos])) = ts[i]
			pos += 4
			*(*uint16)(unsafe.Pointer(&out[pos])) = levels[i]
			pos += 2
			*(*uint16)(unsafe.Pointer(&out[pos])) = lens[i]
			pos += 2
		}
		
		for _, b := range out {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	return hash
}