package main

func solutionBlendByte(s, d, a uint8) uint8 {
	sum := uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127
	return uint8((sum * 0x101) >> 16)
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
	out := make([]byte, len(src))
	var hash uint64
	pixels := width * height
	for iter := 0; iter < iters; iter++ {
		base := 0
		for i := 0; i < pixels; i++ {
			a := src[base+3]
			out[base] = solutionBlendByte(src[base], dst[base], a)
			out[base+1] = solutionBlendByte(src[base+1], dst[base+1], a)
			out[base+2] = solutionBlendByte(src[base+2], dst[base+2], a)
			out[base+3] = 255
			base += 4
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}