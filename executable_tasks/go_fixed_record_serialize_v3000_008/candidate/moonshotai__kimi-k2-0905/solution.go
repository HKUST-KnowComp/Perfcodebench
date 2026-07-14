package main

import (
	"unsafe"
)

const recordBytes = 12

func solutionChecksumBytes(data []byte) uint64 {
	const (
		offset = 1469598103934665603
		prime  = 1099511628211
	)
	var (
		hash64 uint64 = offset
		data64       = (*[1 << 20]uint64)(unsafe.Pointer(&data[0]))
		len64        = len(data) / 8
		left         = len(data) % 8
	)
	for i := 0; i < len64; i++ {
		hash64 ^= data64[i]
		hash64 *= prime
	}
	if left > 0 {
		for i := len(data) - left; i < len(data); i++ {
			hash64 ^= uint64(data[i])
			hash64 *= prime
		}
	}
	return hash64
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	buf := make([]byte, len(ids)*recordBytes)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		b := buf
		for i := range ids {
			// id (4 bytes)
			b[0] = byte(ids[i])
			b[1] = byte(ids[i] >> 8)
			b[2] = byte(ids[i] >> 16)
			b[3] = byte(ids[i] >> 24)

			// ts (4 bytes)
			b[4] = byte(ts[i])
			b[5] = byte(ts[i] >> 8)
			b[6] = byte(ts[i] >> 16)
			b[7] = byte(ts[i] >> 24)

			// level (2 bytes)
			b[8] = byte(levels[i])
			b[9] = byte(levels[i] >> 8)

			// len (2 bytes)
			b[ = byte(lens[i])
			b[11] = byte(lens[i] >> 8)

			b = b[recordBytes:]
		}
		hash = solutionChecksumBytes(buf)
	}
	return hash
}