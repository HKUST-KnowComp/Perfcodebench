package main

import "unsafe"

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
	recordSize := 12
	bufSize := n * recordSize
	out := make([]byte, bufSize)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		pos := 0
		
		for i := 0; i < n; i++ {
			id := ids[i]
			t := ts[i]
			level := levels[i]
			len := lens[i]
			
			*(*uint32)(unsafe.Pointer(&out[pos])) = id
			pos += 4
			*(*uint32)(unsafe.Pointer(&out[pos])) = t
			pos += 4
			*(*uint16)(unsafe.Pointer(&out[pos])) = level
			pos += 2
			*(*uint16)(unsafe.Pointer(&out[pos])) = len
			pos += 2
		}
		
		for _, b := range out {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	return hash
}