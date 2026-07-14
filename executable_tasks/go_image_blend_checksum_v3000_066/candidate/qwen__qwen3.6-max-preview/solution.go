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
	n := pixels * 4
	var hash uint64
	const prime uint64 = 1099511628211
	const offsetBasis uint64 = 1469598103934665603

	for iter := 0; iter < iters; iter++ {
		hash = offsetBasis
		for i := 0; i < n; i += 4 {
			a := uint32(src[i+3])
			da := 255 - a

			v := uint8((uint32(src[i])*a + uint32(dst[i])*da + 127) / 255)
			hash ^= uint64(v)
			hash *= prime

			v = uint8((uint32(src[i+1])*a + uint32(dst[i+1])*da + 127) / 255)
			hash ^= uint64(v)
			hash *= prime

			v = uint8((uint32(src[i+2])*a + uint32(dst[i+2])*da + 127) / 255)
			hash ^= uint64(v)
			hash *= prime

			hash ^= 255
			hash *= prime
		}
	}
	return hash
}