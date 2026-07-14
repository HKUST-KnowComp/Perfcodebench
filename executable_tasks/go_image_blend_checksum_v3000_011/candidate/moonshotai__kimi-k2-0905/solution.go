package main

import (
	"math/bits"
)

const (
	fnvOffset = 1469598103934665603
	fnvPrime  = 1099511628211
)

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	hash := fnvOffset
	for i := 0; i < len(data); i += 4 {
		hash ^= uint64(data[i])
		hash *= fnvPrime
		hash ^= uint64(data[i+1])
		hash *= fnvPrime
		hash ^= uint64(data[i+2])
		hash *= fnvPrime
		hash ^= uint64(data[i+3])
		hash *= fnvPrime
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		j := 0
		for i := 0; i < len(src); i += 4 {
			a := src[i+3]
			sr, sg, sb := src[i], src[i+1], src[i+2]
			dr, dg, db := dst[i], dst[i+1], dst[i+2]

			out[j] = solutionBlendByte(sr, dr, a)
			out[j+1] = solutionBlendByte(sg, dg, a)
			out[j+2] = solutionBlendByte(sb, db, a)
			out[j+3] = 255

			j += 4
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}