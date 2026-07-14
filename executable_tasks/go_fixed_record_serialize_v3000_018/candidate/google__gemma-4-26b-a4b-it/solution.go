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
	
	// Pre-allocate buffer once
	out := make([]byte, totalSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		idx := 0
		for i := 0; i < n; i++ {
			// Manual unrolling of binary.LittleEndian.PutUint32/16 for speed
			id := ids[i]
			out[idx] = byte(id)
			out[idx+1] = byte(id >> 8)
			out[idx+2] = byte(id >> 16)
			out[idx+3] = byte(id >> 24)
			idx += 4

			t := ts[i]
			out[idx] = byte(t)
			out[idx+1] = byte(t >> 8)
			out[idx+2] = byte(t >> 16)
			out[idx+3] = byte(t >> 24)
			idx += 4

			lvl := levels[i]
			out[idx] = byte(lvl)
			out[idx+1] = byte(lvl >> 8)
			idx += 2

			l := lens[i]
			out[idx] = byte(l)
			out[idx+1] = byte(l >> 8)
			idx += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}