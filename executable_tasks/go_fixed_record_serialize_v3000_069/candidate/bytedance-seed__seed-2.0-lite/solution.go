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
	totalBytes := n * 12 // Fixed 12-byte record size: 4+4+2+2
	out := make([]byte, totalBytes)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := range ids {
			off := i * 12
			binary.LittleEndian.PutUint32(out[off:], ids[i])
			binary.LittleEndian.PutUint32(out[off+4:], ts[i])
			binary.LittleEndian.PutUint16(out[off+8:], levels[i])
			binary.LittleEndian.PutUint16(out[off+10:], lens[i])
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}