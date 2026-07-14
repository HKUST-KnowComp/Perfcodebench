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
	bufSize := n * recordSize
	
	// Pre-allocate the buffer once to avoid allocations inside the loop
	out := make([]byte, bufSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		idx := 0
		for i := 0; i < n; i++ {
			// Manual inlining of binary.LittleEndian.PutUint32/16 for performance
			// ID
			out[idx] = byte(ids[i])
			out[idx+1] = byte(ids[i] >> 8)
			out[idx+2] = byte(ids[i] >> 16)
			out[idx+3] = byte(ids[i] >> 24)
			idx += 4

			// TS
			out[idx] = byte(ts[i])
			out[idx+1] = byte(ts[i] >> 8)
			out[idx+2] = byte(ts[i] >> 16)
			out[idx+3] = byte(ts[i] >> 24)
			idx += 4

			// Level
			out[idx] = byte(levels[i])
			out[idx+1] = byte(levels[i] >> 8)
			idx += 2

			// Len
			out[idx] = byte(lens[i])
			out[idx+1] = byte(lens[i] >> 8)
			idx += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}