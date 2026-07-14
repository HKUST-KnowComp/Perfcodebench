package main

import "unsafe"

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	
	// Unroll by 8 for better ILP
	var h0, h1, h2, h3, h4, h5, h6, h7 uint64 = hash, hash, hash, hash, hash, hash, hash, hash
	
	i := 0
	for ; i+7 < len(data); i += 8 {
		h0 ^= uint64(data[i])
		h0 *= prime
		h1 ^= uint64(data[i+1])
		h1 *= prime
		h2 ^= uint64(data[i+2])
		h2 *= prime
		h3 ^= uint64(data[i+3])
		h3 *= prime
		h4 ^= uint64(data[i+4])
		h4 *= prime
		h5 ^= uint64(data[i+5])
		h5 *= prime
		h6 ^= uint64(data[i+6])
		h6 *= prime
		h7 ^= uint64(data[i+7])
		h7 *= prime
	}
	
	// Combine parallel hashes
	hash = h0 ^ h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h7
	
	// Handle remaining bytes
	for ; i < len(data); i++ {
		hash ^= uint64(data[i])
		hash *= prime
	}
	
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	recordSize := 12 // 4 + 4 + 2 + 2
	totalSize := len(ids) * recordSize
	out := make([]byte, totalSize)
	
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		pos := 0
		for i := range ids {
			// Write uint32 ids[i]
			*(*uint32)(unsafe.Pointer(&out[pos])) = ids[i]
			pos += 4
			
			// Write uint32 ts[i]
			*(*uint32)(unsafe.Pointer(&out[pos])) = ts[i]
			pos += 4
			
			// Write uint16 levels[i]
			*(*uint16)(unsafe.Pointer(&out[pos])) = levels[i]
			pos += 2
			
			// Write uint16 lens[i]
			*(*uint16)(unsafe.Pointer(&out[pos])) = lens[i]
			pos += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}