package main

import "unsafe"

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	recordSize := 12
	totalSize := n * recordSize
	out := make([]byte, totalSize)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		// Serialize all records using unsafe pointer arithmetic
		ptr := unsafe.Pointer(&out[0])
		for i := 0; i < n; i++ {
			base := unsafe.Add(ptr, i*recordSize)
			*(*uint32)(base) = ids[i]
			*(*uint32)(unsafe.Add(base, 4)) = ts[i]
			*(*uint16)(unsafe.Add(base, 8)) = levels[i]
			*(*uint16)(unsafe.Add(base, 10)) = lens[i]
		}

		// FNV-1a hash optimized with 8-byte chunks
		hash = 1469598103934665603
		const prime = 1099511628211

		// Process 8 bytes at a time
		chunks := totalSize / 8
		for j := 0; j < chunks; j++ {
			v := *(*uint64)(unsafe.Add(ptr, j*8))
			hash ^= v & 0xFF
			hash *= prime
			hash ^= (v >> 8) & 0xFF
			hash *= prime
			hash ^= (v >> 16) & 0xFF
			hash *= prime
			hash ^= (v >> 24) & 0xFF
			hash *= prime
			hash ^= (v >> 32) & 0xFF
			hash *= prime
			hash ^= (v >> 40) & 0xFF
			hash *= prime
			hash ^= (v >> 48) & 0xFF
			hash *= prime
			hash ^= (v >> 56) & 0xFF
			hash *= prime
		}

		// Handle remaining bytes
		for j := chunks * 8; j < totalSize; j++ {
			hash ^= uint64(out[j])
			hash *= prime
		}
	}
	return hash
}
