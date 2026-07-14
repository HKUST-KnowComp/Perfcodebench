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
	n := pixels * 4
	out := make([]byte, n)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < n; i += 4 {
			a := uint32(src[i+3])
			invA := 255 - a
			out[i] = byte((uint32(src[i])*a + uint32(dst[i])*invA + 127) / 255)
			out[i+1] = byte((uint32(src[i+1])*a + uint32(dst[i+1])*invA + 127) / 255)
			out[i+2] = byte((uint32(src[i+2])*a + uint32(dst[i+2])*invA + 127) / 255)
			out[i+3] = 255
		}

		hash = 1469598103934665603
		for _, b := range out {
			hash ^= uint64(b)
			hash *= 1099511628211
		}
	}
	return hash
}