package main

import "encoding/binary"

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
	// Each record is 4 (id) + 4 (ts) + 2 (level) + 2 (len) = 12 bytes
	recordSize := 12
	totalSize := n * recordSize
	
	// Pre-allocate the buffer once
	out := make([]byte, totalSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		idx := 0
		for i := 0; i < n; i++ {
			// Manual unrolled serialization for speed
			// ID (uint32)
			out[idx] = byte(ids[i])
			out[idx+1] = byte(ids[i] >> 8)
			out[idx+2] = byte(ids[i] >> 16)
			out[idx+3] = byte(ids[i] >> 24)
			
			// TS (uint32)
			out[idx+4] = byte(ts[i])
			out[idx+5] = byte(ts[i] >> 8)
			out[idx+6] = byte(ts[i] >> 16)
			out[idx+7] = byte(ts[i] >> 24)
			
			// Level (uint16)
			out[idx+8] = byte(levels[i])
			out[idx+9] = byte(levels[i] >> 8)
			
			// Lens (uint16)
			out[idx+10] = byte(lens[i])
			out[idx+11] = byte(lens[i] >> 8)
			
			idx += 12
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
