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
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			invA := uint32(255 - a)
			a32 := uint32(a)
			// blend R
			r := uint8((uint32(src[base])*a32 + uint32(dst[base])*invA + 127) / 255)
			hash ^= uint64(r)
			hash *= 1099511628211
			// blend G
			g := uint8((uint32(src[base+1])*a32 + uint32(dst[base+1])*invA + 127) / 255)
			hash ^= uint64(g)
			hash *= 1099511628211
			// blend B
			b := uint8((uint32(src[base+2])*a32 + uint32(dst[base+2])*invA + 127) / 255)
			hash ^= uint64(b)
			hash *= 1099511628211
			// A is 255
			hash ^= 255
			hash *= 1099511628211
		}
	}
	return hash
}
