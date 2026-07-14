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
	totalBufSize := n * 12 // 4B id +4B ts +2B levels +2B lens per record
	out := make([]byte, 0, totalBufSize) // Preallocate full capacity once
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		out = out[:0] // Reset length, preserve backing capacity
		for i := range ids {
			// Write 4-byte id
			out = out[:len(out)+4]
			binary.LittleEndian.PutUint32(out[len(out)-4:], ids[i])
			// Write 4-byte timestamp
			out = out[:len(out)+4]
			binary.LittleEndian.PutUint32(out[len(out)-4:], ts[i])
			// Write 2-byte level
			out = out[:len(out)+2]
			binary.LittleEndian.PutUint16(out[len(out)-2:], levels[i])
			// Write 2-byte length
			out = out[:len(out)+2]
			binary.LittleEndian.PutUint16(out[len(out)-2:], lens[i])
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}