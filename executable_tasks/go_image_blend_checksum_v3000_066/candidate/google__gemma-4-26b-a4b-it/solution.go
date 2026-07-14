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
			s0 := src[base]
			s1 := src[base+1]
			s2 := src[base+2]
			a := src[base+3]

			d0 := dst[base]
			d1 := dst[base+1]
			d2 := dst[base+2]

			// Inline solutionBlendByte for performance
			ua := uint32(a)
			invA := 255 - ua

			out[base] = uint8((uint32(s0)*ua + uint32(d0)*invA + 127) / 255)
			out[base+1] = uint8((uint32(s1)*ua + uint32(d1)*invA + 127) / 255)
			out[base+2] = uint8((uint32(s2)*ua + uint32(d2)*invA + 127) / 255)
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}