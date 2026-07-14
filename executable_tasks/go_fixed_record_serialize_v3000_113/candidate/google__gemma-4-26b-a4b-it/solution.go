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
	
	out := make([]byte, totalSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		idx := 0
		for i := 0; i < n; i++ {
			// Manual little-endian encoding to avoid function call overhead and bounds checks
			// ID
			idVal := ids[i]
			out[idx] = byte(idVal)
			out[idx+1] = byte(idVal >> 8)
			out[idx+2] = byte(idVal >> 16)
			out[idx+3] = byte(idVal >> 24)
			idx += 4

			// TS
			tsVal := ts[i]
			out[idx] = byte(tsVal)
			out[idx+1] = byte(tsVal >> 8)
			out[idx+2] = byte(tsVal >> 16)
			out[idx+3] = byte(tsVal >> 24)
			idx += 4

			// Level
			lvlVal := levels[i]
			out[idx] = byte(lvlVal)
			out[idx+1] = byte(lvlVal >> 8)
			idx += 2

			// Len
			lenVal := lens[i]
			out[idx] = byte(lenVal)
			out[idx+1] = byte(lenVal >> 8)
			idx += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}