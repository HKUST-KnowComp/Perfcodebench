package main

import "unsafe"

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	len := len(data)
	i := 0
	
	// Unroll by 8 for better ILP
	for i+7 < len {
		hash ^= uint64(data[i])
		hash *= 1099511628211
		hash ^= uint64(data[i+1])
		hash *= 1099511628211
		hash ^= uint64(data[i+2])
		hash *= 1099511628211
		hash ^= uint64(data[i+3])
		hash *= 1099511628211
		hash ^= uint64(data[i+4])
		hash *= 1099511628211
		hash ^= uint64(data[i+5])
		hash *= 1099511628211
		hash ^= uint64(data[i+6])
		hash *= 1099511628211
		hash ^= uint64(data[i+7])
		hash *= 1099511628211
		i += 8
	}
	
	// Handle remaining bytes
	for i < len {
		hash ^= uint64(data[i])
		hash *= 1099511628211
		i++
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	recordSize := 12 // 4 + 4 + 2 + 2 bytes per record
	totalSize := len(ids) * recordSize
	out := make([]byte, totalSize)
	var hash uint64
	
	for iter := 0; iter < iters; iter++ {
		pos := 0
		for i := range ids {
			// Write ids[i] as 4 bytes
			*(*uint32)(unsafe.Pointer(&out[pos])) = ids[i]
			pos += 4
			
			// Write ts[i] as 4 bytes
			*(*uint32)(unsafe.Pointer(&out[pos])) = ts[i]
			pos += 4
			
			// Write levels[i] as 2 bytes
			*(*uint16)(unsafe.Pointer(&out[pos])) = levels[i]
			pos += 2
			
			// Write lens[i] as 2 bytes
			*(*uint16)(unsafe.Pointer(&out[pos])) = lens[i]
			pos += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
