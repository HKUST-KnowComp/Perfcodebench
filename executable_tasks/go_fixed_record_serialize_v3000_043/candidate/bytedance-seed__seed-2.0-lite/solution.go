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
	totalBufLen := n * 12
	out := make([]byte, totalBufLen)
	var hash uint64
	const recordStride = 12

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			off := i * recordStride
			binary.LittleEndian.PutUint32(out[off:], ids[i])
			binary.LittleEndian.PutUint32(out[off+4:], ts[i])
			binary.LittleEndian.PutUint16(out[off+8:], levels[i])
			binary.LittleEndian.PutUint16(out[off+10:], lens[i])
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}