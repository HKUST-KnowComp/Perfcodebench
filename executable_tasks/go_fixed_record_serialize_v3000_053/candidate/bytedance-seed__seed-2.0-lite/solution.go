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
	if n == 0 {
		var hash uint64
		for iter := 0; iter < iters; iter++ {
			hash = solutionChecksumBytes(nil)
		}
		return hash
	}
	// Preallocate full buffer once (12 bytes per record: 4+4+2+2)
	out := make([]byte, 0, n*12)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		out = out[:0]
		offset := 0
		for i := 0; i < n; i++ {
			// Write all fields in original order, using optimized little-endian writes
			binary.LittleEndian.PutUint32(out[offset:], ids[i])
			offset += 4
			binary.LittleEndian.PutUint32(out[offset:], ts[i])
			offset += 4
			binary.LittleEndian.PutUint16(out[offset:], levels[i])
			offset += 2
			binary.LittleEndian.PutUint16(out[offset:], lens[i])
			offset += 2
		}
		out = out[:offset]
		hash = solutionChecksumBytes(out)
	}
	return hash
}