package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for i := 0; i < len(data); i++ {
		hash ^= uint64(data[i])
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	const (
		fnvOffset uint64 = 1469598103934665603
		fnvPrime  uint64 = 1099511628211
	)

	n := width * height * 4
	out := make([]byte, n)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for base := 0; base < n; base += 4 {
			a := src[base+3]
			ia := uint32(a)
			invA := uint32(255 - a)

			b0 := uint8((uint32(src[base])*ia + uint32(dst[base])*invA + 127) / 255)
			b1 := uint8((uint32(src[base+1])*ia + uint32(dst[base+1])*invA + 127) / 255)
			b2 := uint8((uint32(src[base+2])*ia + uint32(dst[base+2])*invA + 127) / 255)

			out[base] = b0
			out[base+1] = b1
			out[base+2] = b2
			out[base+3] = 255

			h ^= uint64(b0)
			h *= fnvPrime
			h ^= uint64(b1)
			h *= fnvPrime
			h ^= uint64(b2)
			h *= fnvPrime
			h ^= 255
			h *= fnvPrime
		}
		hash = h
	}

	return hash
}
