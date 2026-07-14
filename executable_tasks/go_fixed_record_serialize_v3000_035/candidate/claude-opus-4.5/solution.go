package main

import (
	"encoding/binary"
)

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	recordSize := 12 // 4 + 4 + 2 + 2 bytes per record
	totalSize := n * recordSize
	
	// Pre-allocate buffer once
	out := make([]byte, totalSize)
	
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		// Serialize all records using direct memory writes
		offset := 0
		for i := 0; i < n; i++ {
			binary.LittleEndian.PutUint32(out[offset:], ids[i])
			binary.LittleEndian.PutUint32(out[offset+4:], ts[i])
			binary.LittleEndian.PutUint16(out[offset+8:], levels[i])
			binary.LittleEndian.PutUint16(out[offset+10:], lens[i])
			offset += recordSize
		}
		
		// FNV-1a hash with loop unrolling
		h := uint64(1469598103934665603)
		const prime = 1099511628211
		
		// Process 8 bytes at a time
		i := 0
		for ; i+7 < totalSize; i += 8 {
			h ^= uint64(out[i])
			h *= prime
			h ^= uint64(out[i+1])
			h *= prime
			h ^= uint64(out[i+2])
			h *= prime
			h ^= uint64(out[i+3])
			h *= prime
			h ^= uint64(out[i+4])
			h *= prime
			h ^= uint64(out[i+5])
			h *= prime
			h ^= uint64(out[i+6])
			h *= prime
			h ^= uint64(out[i+7])
			h *= prime
		}
		
		// Handle remaining bytes
		for ; i < totalSize; i++ {
			h ^= uint64(out[i])
			h *= prime
		}
		
		hash = h
	}
	return hash
}