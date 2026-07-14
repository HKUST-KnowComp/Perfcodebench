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
	var hash uint64
	limit := width * height * 4
	prime := uint64(1099511628211)

	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < limit; i += 4 {
			a := uint32(src[i+3])
			invA := uint32(255) - a

			r := uint8((uint32(src[i])*a + uint32(dst[i])*invA + 127) / 255)
			g := uint8((uint32(src[i+1])*a + uint32(dst[i+1])*invA + 127) / 255)
			b := uint8((uint32(src[i+2])*a + uint32(dst[i+2])*invA + 127) / 255)

			hash ^= uint64(r)
			hash *= prime
			hash ^= uint64(g)
			hash *= prime
			hash ^= uint64(b)
			hash *= prime
			hash ^= 255
			hash *= prime
		}
	}
	return hash
}