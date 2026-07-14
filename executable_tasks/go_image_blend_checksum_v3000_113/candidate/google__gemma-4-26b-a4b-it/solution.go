package main

func solutionBlendByte(s, d, a uint8) uint8 {
	// Optimized blending: (s*a + d*(255-a) + 127) / 255
	// Using the approximation (val + 1 + (val >> 8)) >> 8 or similar is common,
	// but for exactness with the original formula:
	val := uint32(s)*uint32(a) + uint32(d)*(255-uint32(a))
	return uint8((val + 127) / 255)
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
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := src[base+3]

			// Inline blending for performance
			// R
			sR := uint32(src[base])*uint32(a) + uint32(dst[base])*(255-uint32(a))
			out[base] = uint8((sR + 127) / 255)
			// G
			sG := uint32(src[base+1])*uint32(a) + uint32(dst[base+1])*(255-uint32(a))
			out[base+1] = uint8((sG + 127) / 255)
			// B
			sB := uint32(src[base+2])*uint32(a) + uint32(dst[base+2])*(255-uint32(a))
			out[base+2] = uint8((sB + 127) / 255)
			// A
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
