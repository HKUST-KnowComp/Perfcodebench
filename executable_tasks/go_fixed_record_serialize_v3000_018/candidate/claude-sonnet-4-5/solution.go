package main

import "unsafe"

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	
	i := 0
	n := len(data)
	
	// Unroll by 8
	for i+7 < n {
		hash ^= uint64(data[i])
		hash *= prime
		hash ^= uint64(data[i+1])
		hash *= prime
		hash ^= uint64(data[i+2])
		hash *= prime
		hash ^= uint64(data[i+3])
		hash *= prime
		hash ^= uint64(data[i+4])
		hash *= prime
		hash ^= uint64(data[i+5])
		hash *= prime
		hash ^= uint64(data[i+6])
		hash *= prime
		hash ^= uint64(data[i+7])
		hash *= prime
		i += 8
	}
	
	// Handle remainder
	for i < n {
		hash ^= uint64(data[i])
		hash *= prime
		i++
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
			*(*uint32)(unsafe.Pointer(&out[pos])) = ids[i]
			pos += 4
			*(*uint32)(unsafe.Pointer(&out[pos])) = ts[i]
			pos += 4
			*(*uint16)(unsafe.Pointer(&out[pos])) = levels[i]
			pos += 2
			*(*uint16)(unsafe.Pointer(&out[pos])) = lens[i]
			pos += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
