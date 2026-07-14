package main

import "encoding/binary"

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const recordSize = 12
	buf := make([]byte, len(ids)*recordSize)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < len(ids); i++ {
			offset := i * recordSize
			binary.LittleEndian.PutUint32(buf[offset:], ids[i])
			binary.LittleEndian.PutUint32(buf[offset+4:], ts[i])
			binary.LittleEndian.PutUint16(buf[offset+8:], levels[i])
			binary.LittleEndian.PutUint16(buf[offset+10:], lens[i])
		}

		for _, b := range buf {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	return hash
}