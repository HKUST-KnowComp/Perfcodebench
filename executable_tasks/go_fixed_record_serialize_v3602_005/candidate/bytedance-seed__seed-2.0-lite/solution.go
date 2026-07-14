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
	totalBytes := len(ids) * 12
	out := make([]byte, totalBytes)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := range ids {
			start := i * 12
			binary.LittleEndian.PutUint32(out[start:], ids[i])
			binary.LittleEndian.PutUint32(out[start+4:], ts[i])
			binary.LittleEndian.PutUint16(out[start+8:], levels[i])
			binary.LittleEndian.PutUint16(out[start+10:], lens[i])
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}