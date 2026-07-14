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
	if n == 0 || iters <= 0 {
		return 0
	}
	const recordSize = 12
	out := make([]byte, n*recordSize)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i++ {
			off := i * recordSize
			buf := out[off:]
			binary.LittleEndian.PutUint32(buf[0:4], ids[i])
			binary.LittleEndian.PutUint32(buf[4:8], ts[i])
			binary.LittleEndian.PutUint16(buf[8:10], levels[i])
			binary.LittleEndian.PutUint16(buf[10:12], lens[i])
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}