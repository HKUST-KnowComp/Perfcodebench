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
			
			// Inline solutionBlendByte for performance
			// s = src[base], d = dst[base]
			
			// R
			sR := uint32(src[base])
			dR := uint32(dst[base])
			out[base] = uint8((sR*uint32(a) + dR*(255-uint32(a)) + 127) / 255)
			
			// G
			sG := uint32(src[base+1])
			dG := uint32(dst[base+1])
			out[base+1] = uint8((sG*uint32(a) + dG*(255-uint32(a)) + 127) / 255)
			
			// B
			sB := uint32(src[base+2])
			dB := uint32(dst[base+2])
			out[base+2] = uint8((sB*uint32(a) + dB*(255-uint32(a)) + 127) / 255)
			
			// A
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}