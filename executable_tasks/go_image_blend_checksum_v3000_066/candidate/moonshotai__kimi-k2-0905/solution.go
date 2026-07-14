package main

import (
	"math/bits"
)

func run(src, dst []byte, width, height, iters int) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
	)
	pixels := width * height
	buf := make([]byte, pixels*4)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		// Blend whole image
		for i := 0; i < pixels; {
			// Unroll 4 pixels per iteration
			for j := 0; j < 4 && i < pixels; j++ {
				off := i * 4
				a := src[off+3]

				// Compute blends
				r := (uint32(src[off])*uint32(a) + uint32(dst[off])*uint32(255-a) + 127) / 255
				g := (uint32(src[off+1])*uint32(a) + uint32(dst[off+1])*uint32(255-a) + 127) / 255
				b := (uint32(src[off+2])*uint32(a) + uint32(dst[off+2])*uint32(255-a) + 127) / 255

				buf[off] = uint8(r)
				buf[off+1] = uint8(g)
				buf[off+2] = uint8(b)
				buf[off+3] = 255
				i++
			}
		}

		// 64-bit aligned FNV-1a checksum
		hash = fnvOffset
		l := len(buf)
		for k := 0; k < l-7; k += 8 {
			v := uint64(buf[k]) | uint64(buf[k+1])<<8 | uint64(buf[k+2])<<16 | uint64(buf[k+3])<<24 |
				uint64(buf[k+4])<<32 | uint64(buf[k+5])<<40 | uint64(buf[k+6])<<48 | uint64(buf[k+7])<<56
			hash ^= v
			hash *= fnvPrime
		}
		// Handle tail < 8 bytes
		for k := l & ^7; k < l; k++ {
			hash ^= uint64(buf[k])
			hash *= fnvPrime
		}
	}
	return hash
}