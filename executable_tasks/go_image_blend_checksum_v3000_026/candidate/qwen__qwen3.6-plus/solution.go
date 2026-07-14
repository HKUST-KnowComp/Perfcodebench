package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a))*257 + 32768 >> 16)
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
	const fnvPrime = 1099511628211
	const fnvOffset = 1469598103934665603

	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			invA := 255 - a
			a32 := uint32(a)
			invA32 := uint32(invA)

			val := uint32(src[base])*a32 + uint32(dst[base])*invA32
			hash ^= uint64(uint8((val*257 + 32768) >> 16))
			hash *= fnvPrime

			val = uint32(src[base+1])*a32 + uint32(dst[base+1])*invA32
			hash ^= uint64(uint8((val*257 + 32768) >> 16))
			hash *= fnvPrime

			val = uint32(src[base+2])*a32 + uint32(dst[base+2])*invA32
			hash ^= uint64(uint8((val*257 + 32768) >> 16))
			hash *= fnvPrime

			hash ^= 255
			hash *= fnvPrime
		}
	}
	return hash
}