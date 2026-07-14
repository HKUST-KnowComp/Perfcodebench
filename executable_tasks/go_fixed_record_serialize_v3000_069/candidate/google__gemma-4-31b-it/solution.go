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
		pos := 0
		for i := 0; i < numRecords; i++ {
			id := ids[i]
			t := ts[i]
			lvl := levels[i]
			ln := lens[i]

			// Serialize
			binary.LittleEndian.PutUint32(buf[pos:], id)
			binary.LittleEndian.PutUint32(buf[pos+4:], t)
			binary.LittleEndian.PutUint16(buf[pos+8:], lvl)
			binary.LittleEndian.PutUint16(buf[pos+10:], ln)
			pos += recordSize
		}

		// Checksum
		hash = 1469598103934665603
		for _, b := range buf {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	return hash
}