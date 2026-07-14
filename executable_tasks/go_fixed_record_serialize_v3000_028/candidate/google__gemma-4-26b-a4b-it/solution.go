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
	const recordSize = 12
	buf := make([]byte, n*recordSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			offset := i * recordSize
			// Manual inlining of binary.LittleEndian for performance
			buf[offset] = byte(ids[i])
			buf[offset+1] = byte(ids[i] >> 8)
			buf[offset+2] = byte(ids[i] >> 16)
			buf[offset+3] = byte(ids[i] >> 24)

			buf[offset+4] = byte(ts[i])
			buf[offset+5] = byte(ts[i] >> 8)
			buf[offset+6] = byte(ts[i] >> 16)
			buf[offset+7] = byte(ts[i] >> 24)

			buf[offset+8] = byte(levels[i])
			buf[offset+9] = byte(levels[i] >> 8)

			buf[offset+10] = byte(lens[i])
			buf[offset+11] = byte(lens[i] >> 8)
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}