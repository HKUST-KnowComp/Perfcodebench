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
	out := make([]byte, len(src))
	var hash uint64
	pixels := width * height
	for iter := 0; iter < iters; iter++ {
		// process four pixels per internal loop iteration
		for i := 0; i < pixels; i += 4 {
			base := i * 4
			// pixel 0
			a0 := src[base+3]
			out[base] = solutionBlendByte(src[base], dst[base], a0)
			out[base+1] = solutionBlendByte(src[base+1], dst[base+1], a0)
			out[base+2] = solutionBlendByte(src[base+2], dst[base+2], a0)
			out[base+3] = 255
			// pixel 1
			a1 := src[base+7]
			out[base+4] = solutionBlendByte(src[base+4], dst[base+4], a1)
			out[base+5] = solutionBlendByte(src[base+5], dst[base+5], a1)
			out[base+6] = solutionBlendByte(src[base+6], dst[base+6], a1)
			out[base+7] = 255
			// pixel 2
			a2 := src[base+11]
			out[base+8] = solutionBlendByte(src[base+8], dst[base+8], a2)
			out[base+9] = solutionBlendByte(src[base+9], dst[base+9], a2)
			out[base+10] = solutionBlendByte(src[base+10], dst[base+10], a2)
			out[base+11] = 255
			// pixel 3
			a3 := src[base+15]
			out[base+12] = solutionBlendByte(src[base+12], dst[base+12], a3)
			out[base+13] = solutionBlendByte(src[base+13], dst[base+13], a3)
			out[base+14] = solutionBlendByte(src[base+14], dst[base+14], a3)
			out[base+15] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}