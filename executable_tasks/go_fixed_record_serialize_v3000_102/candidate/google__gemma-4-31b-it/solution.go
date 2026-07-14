package main

import "encoding/binary"

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const prime = 1099511628211
	const seed = 1469598103934665603
	
	numRecords := len(ids)
	// Each record is 4 + 4 + 2 + 2 = 12 bytes
	buf := make([]byte, numRecords*12)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		pos := 0
		for i := 0; i < numRecords; i++ {
			binary.LittleEndian.PutUint32(buf[pos:], ids[i])
			binary.LittleEndian.PutUint32(buf[pos+4:], ts[i])
			binary.LittleEndian.PutUint16(buf[pos+8:], levels[i])
			binary.LittleEndian.PutUint16(buf[pos+10:], lens[i])
			pos += 12
		}

		// Calculate checksum on the buffer
		hash = seed
		for _, b := range buf[:pos] {
			hash ^= uint64(b)
			hash *= prime
		}
	}
	return hash
}