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
	recordCount := len(ids)
	totalBytes := recordCount * 12
	out := make([]byte, totalBytes)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		offset := 0
		for i := range ids {
			binary.LittleEndian.PutUint32(out[offset:], ids[i])
			offset += 4
			binary.LittleEndian.PutUint32(out[offset:], ts[i])
			offset +=4
			binary.LittleEndian.PutUint16(out[offset:], levels[i])
			offset +=2
			binary.LittleEndian.PutUint16(out[offset:], lens[i])
			offset +=2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}