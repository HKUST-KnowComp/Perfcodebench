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
		for i := 0; i < iters; i++ {
			hash = solutionChecksumBytes(nil)
		}
		return hash
	}
	totalBufSize := n * 12
	out := make([]byte, totalBufSize)
	var finalHash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			offset := i * 12
			binary.LittleEndian.PutUint32(out[offset:], ids[i])
			binary.LittleEndian.PutUint32(out[offset+4:], ts[i])
			binary.LittleEndian.PutUint16(out[offset+8:], levels[i])
			binary.LittleEndian.PutUint16(out[offset+10:], lens[i])
		}
		finalHash = solutionChecksumBytes(out)
	}
	return finalHash
}