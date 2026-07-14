package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for i := 0; i < len(data); i++ {
		hash ^= uint64(data[i])
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
		for i := 0; i < pixels*4; i += 4 {
			a := uint32(src[i+3])
			invA := 255 - a

			out[j] = uint8((uint32(src[i])*a + uint32(dst[i])*invA + 127) / 255)
			out[j+1] = uint8((uint32(src[i+1])*a + uint32(dst[i+1])*invA + 127) / 255)
			out[j+2] = uint8((uint32(src[i+2])*a + uint32(dst[i+2])*invA + 127) / 255)
			out[j+3] = 255
			j += 4
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
