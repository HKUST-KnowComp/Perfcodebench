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
	// Wait, the original code appends: 4 bytes (id), 4 bytes (ts), 2 bytes (level), 2 bytes (lens) = 12 bytes total.
	// Let's re-verify the original append logic:
	// out = append(out, byte(ids[i]), byte(ids[i]>>8), byte(ids[i]>>16), byte(ids[i]>>24)) -> 4 bytes
	// out = append(out, byte(ts[i]), byte(ts[i]>>8), byte(ts[i]>>16), byte(ts[i]>>24)) -> 4 bytes
	// out = append(out, byte(levels[i]), byte(levels[i]>>8), byte(lens[i]), byte(lens[i]>>8)) -> 4 bytes
	// Total per record = 12 bytes.
	
	const recordSize = 12
	buf := make([]byte, n*recordSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		idx := 0
		for i := 0; i < n; i++ {
			// Manual unrolling of binary.LittleEndian.PutUint32/16 for speed
			id := ids[i]
			t := ts[i]
			l := levels[i]
			n_val := lens[i]

			buf[idx] = byte(id)
			buf[idx+1] = byte(id >> 8)
			buf[idx+2] = byte(id >> 16)
			buf[idx+3] = byte(id >> 24)

			buf[idx+4] = byte(t)
			buf[idx+5] = byte(t >> 8)
			buf[idx+6] = byte(t >> 16)
			buf[idx+7] = byte(t >> 24)

			buf[idx+8] = byte(l)
			buf[idx+9] = byte(l >> 8)
			buf[idx+10] = byte(n_val)
			buf[idx+11] = byte(n_val >> 8)

			idx += recordSize
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}