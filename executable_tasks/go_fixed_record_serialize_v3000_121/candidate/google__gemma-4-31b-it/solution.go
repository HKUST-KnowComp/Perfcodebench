package main

import (
	"encoding/binary"
)

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	numRecords := len(ids)
	recordSize := 12 // 4 + 4 + 2 + 2
	buf := make([]byte, numRecords*recordSize)
	
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < numRecords; i++ {
			off := i * recordSize
			binary.LittleEndian.PutUint32(buf[off:], ids[i])
			binary.LittleEndian.PutUint32(buf[off+4:], ts[i])
			binary.LittleEndian.PutUint16(buf[off+8:], levels[i])
			binary.LittleEndian.PutUint16(buf[off+10:], lens[i])
		}
		
		// Inline checksum calculation to avoid extra pass
		hash = 1469598103934665603
		for _, b := range buf {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	return hash
}