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
			si := i * 4
			a := src[si+3]

			out[j] = solutionBlendByte(src[si], dst[si], a)
			out[j+1] = solutionBlendByte(src[si+1], dst[si+1], a)
			out[j+2] = solutionBlendByte(src[si+2], dst[si+2], a)
			out[j+3] = 255

			j += 4
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}