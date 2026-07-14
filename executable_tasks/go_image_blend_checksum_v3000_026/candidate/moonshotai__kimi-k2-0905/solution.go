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
	out := make([]byte, 4*width*height)
	pixels := width * height
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			srcBase := i * 4
			dstBase := i * 4
			outBase := i * 4

			a := src[srcBase+3]
			out[outBase] = solutionBlendByte(src[srcBase], dst[dstBase], a)
			out[outBase+1] = solutionBlendByte(src[srcBase+1], dst[dstBase+1], a)
			out[outBase+2] = solutionBlendByte(src[srcBase+2], dst[dstBase+2], a)
			out[outBase+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
