package main

import "encoding/binary"

func solutionChecksumBytes(data []byte) uint64 {
	hash := uint64(1469598103934665603)
	for i := 0; i < len(data); i += 32 {
		for j := 0; j < 32 && i+j < len(data); j++ {
			hash ^= uint64(data[i+j])
			hash *= 1099511628211
		}
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	rec := len(ids)
	bufSize := rec * 12
	buf := make([]byte, bufSize)
	hash := uint64(0)
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < rec; i++ {
			off := i * 12
			binary.LittleEndian.PutUint32(buf[off:], ids[i])
			binary.LittleEndian.PutUint32(buf[off+4:], ts[i])
			binary.LittleEndian.PutUint16(buf[off+8:], levels[i])
			binary.LittleEndian.PutUint16(buf[off+10:], lens[i])
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}