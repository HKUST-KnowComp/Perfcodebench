package main

func solutionBlendByte(s, d, a uint32) uint8 {
	return uint8((s*a + d*(255-a) + 127) / 255)
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
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			out[base] = solutionBlendByte(uint32(src[base]), uint32(dst[base]), a)
			out[base+1] = solutionBlendByte(uint32(src[base+1]), uint32(dst[base+1]), a)
			out[base+2] = solutionBlendByte(uint32(src[base+2]), uint32(dst[base+2]), a)
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}