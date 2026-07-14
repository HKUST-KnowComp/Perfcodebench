package main

import (
	"encoding/binary"
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
	const recordSize = 10 // 4 (id) + 4 (ts) + 2 (level) + 2 (len) is not correct based on the original code logic
	// Re-evaluating original logic: 
	// id (4) + ts (4) + level (2) + lens (2) = 12 bytes per record
	const actualRecordSize = 12
	
	var out []byte
	var hash uint64

	// Pre-allocate buffer once to avoid allocations in the loop
	out = make([]byte, n*actualRecordSize)

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			offset := i * actualRecordSize
			
			// Use binary.LittleEndian for optimized writes
			binary.LittleEndian.PutUint32(out[offset:offset+4], ids[i])
			binary.LittleEndian.PutUint32(out[offset+4:offset+8], ts[i])
			binary.LittleEndian.PutUint16(out[offset+8:offset+10], levels[i])
			binary.LittleEndian.PutUint16(out[offset+10:offset+12], lens[i])
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}