package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		j := 0
		for i := 0; i < pixels; i++ {
			off := i * 4
			a := src[off+3]

			s0, d0 := src[off], dst[off]
			s1, d1 := src[off+1], dst[off+1]
			s2, d2 := src[off+2], dst[off+2]

			out[j] = solutionBlendByte(s0, d0, a)
			out[j+1] = solutionBlendByte(s1, d1, a)
			out[j+2] = solutionBlendByte(s2, d2, a)
			out[j+3] = 255

			j += 4
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}