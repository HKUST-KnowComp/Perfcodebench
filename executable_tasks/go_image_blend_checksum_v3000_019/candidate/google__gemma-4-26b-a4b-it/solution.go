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
	outLen := pixels * 4
	out := make([]byte, outLen)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := src[base+3]
			
			// Exact math required by correctness rule: (s*a + d*(255-a) + 127) / 255
			// We must use the exact formula to ensure checksum matches.
			
			s0, d0 := uint32(src[base]), uint32(dst[base])
			s1, d1 := uint32(src[base+1]), uint32(dst[base+1])
			s2, d2 := uint32(src[base+2]), uint32(dst[base+2])
			
			invA := 255 - uint32(a)
			
			out[base] = uint8((s0*uint32(a) + d0*invA + 127) / 255)
			out[base+1] = uint8((s1*uint32(a) + d1*invA + 127) / 255)
			out[base+2] = uint8((s2*uint32(a) + d2*invA + 127) / 255)
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}
