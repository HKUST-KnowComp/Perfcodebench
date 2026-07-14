package main

import "math/bits"

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	const (
		offset64 uint64 = 14695981039346656037
		prime64  uint64 = 1099511628211
	)
	hash := offset64
	for _, v := range data {
		hash ^= uint64(v)
		hash *= prime64
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	out := make([]byte, len(src))
	pixels := width * height
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		j := 0
		for i := 0; i < pixels; i++ {
			off := i * 4
			a := src[off+3]

			if a == 255 {
				// fast-path: source is opaque
				out[j+0] = src[off+0]
				out[j+1] = src[off+1]
				out[j+2] = src[off+2]
				out[j+3] = 255
			} else {
				s0, d0 := src[off+0], dst[off+0]
				s1, d1 := src[off+1], dst[off+1]
				s2, d2 := src[off+2], dst[off+2]

				out[j+0] = uint8((uint32(s0)*uint32(a) + uint32(d0)*uint32(255-a) + 127) / 255
				out[j+1] = uint8((uint32(s1)*uint32(a) + uint32(d1)*uint32(255-a) + 127) / 255
				out[j+2] = uint8((uint32(s2)*uint32(a) + uint32(d2)*uint32(255-a) + 127) / 255
				out[j+3] = 255
			}
			j += 4
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
