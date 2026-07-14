package main

import (
	"encoding/binary"
)

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	numRecords := len(ids)
	recordSize := 4 + 4 + 2 + 2
	totalSize := numRecords * recordSize
	out := make([]byte, totalSize)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		pos := 0
		for i := 0; i < numRecords; i++ {
			binary.LittleEndian.PutUint32(out[pos:], ids[i])
			binary.LittleEndian.PutUint32(out[pos+4:], ts[i])
			binary.LittleEndian.PutUint16(out[pos+8:], levels[i])
			binary.LittleEndian.PutUint16(out[pos+10:], lens[i])
			pos += recordSize
		}

		// Inline checksum calculation to avoid extra pass
		hash = 1469598103934665603
		for _, b := range out {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	return hash
}