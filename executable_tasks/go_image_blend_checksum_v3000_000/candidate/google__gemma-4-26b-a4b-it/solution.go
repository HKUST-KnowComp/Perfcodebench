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
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := src[base+3]
			
			// Pre-calculate alpha components to avoid repeated work
			a32 := uint32(a)
			invA32 := 255 - a32
			
			// Inline blending to avoid function call overhead and use uint32 math
			// Formula: (s*a + d*(255-a) + 127) / 255
			
			s0 := uint32(src[base])
			d0 := uint32(dst[base])
			out[base] = uint8((s0*a32 + d0*invA32 + 127) / 255)
		
			s1 := uint32(src[base+1])
			d1 := uint32(dst[base+1])
			out[base+1] = uint8((s1*a32 + d1*invA32 + 127) / 255)
		
			s2 := uint32(src[base+2])
			d2 := uint32(dst[base+2])
			out[base+2] = uint8((s2*a32 + d2*invA32 + 127) / 255)
		
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}