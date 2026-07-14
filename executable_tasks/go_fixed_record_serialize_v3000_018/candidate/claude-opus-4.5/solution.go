package main

import "unsafe"

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	n := len(data)
	i := 0
	// Process 8 bytes at a time
	for ; i+7 < n; i += 8 {
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
	}
	// Handle remaining bytes
	for ; i < n; i++ {
		hash ^= uint64(data[i])
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	recordSize := 12 // 4 + 4 + 2 + 2 bytes per record
	n := len(ids)
	totalSize := n * recordSize
	out := make([]byte, totalSize)
	
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		// Serialize all records using direct memory writes
		offset := 0
		for i := 0; i < n; i++ {
			// Write id (4 bytes, little-endian)
			*(*uint32)(unsafe.Pointer(&out[offset])) = ids[i]
			offset += 4
			// Write ts (4 bytes, little-endian)
			*(*uint32)(unsafe.Pointer(&out[offset])) = ts[i]
			offset += 4
			// Write level (2 bytes, little-endian)
			*(*uint16)(unsafe.Pointer(&out[offset])) = levels[i]
			offset += 2
			// Write len (2 bytes, little-endian)
			*(*uint16)(unsafe.Pointer(&out[offset])) = lens[i]
			offset += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}