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
	recordSize := 12
	totalSize := n * recordSize
	out := make([]byte, totalSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			offset := i * recordSize
			binary.LittleEndian.PutUint32(out[offset:], ids[i])
			binary.LittleEndian.PutUint32(out[offset+4:], ts[i])
			binary.LittleEndian.PutUint16(out[offset+8:], levels[i])
			binary.LittleEndian.PutUint16(out[offset+10:], lens[i])
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}