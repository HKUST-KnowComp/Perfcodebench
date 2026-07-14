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
	const fnvPrime = 1099511628211
	const fnvOffset = 1469598103934665603
	n := width * height * 4
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := 0; i < n; i += 4 {
			a := uint32(src[i+3])
			invA := 255 - a

			v := uint32(src[i])*a + uint32(dst[i])*invA
			hash ^= uint64(byte((v + 127) / 255))
			hash *= fnvPrime

			v = uint32(src[i+1])*a + uint32(dst[i+1])*invA
			hash ^= uint64(byte((v + 127) / 255))
			hash *= fnvPrime

			v = uint32(src[i+2])*a + uint32(dst[i+2])*invA
			hash ^= uint64(byte((v + 127) / 255))
			hash *= fnvPrime

			hash ^= uint64(255)
			hash *= fnvPrime
		}
	}
	return hash
}