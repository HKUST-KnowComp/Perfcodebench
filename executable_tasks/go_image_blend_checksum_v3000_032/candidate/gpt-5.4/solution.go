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
	pixels := width * height
	out := make([]byte, pixels*4)

	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := offset
		for i, j := 0, 0; i < pixels; i, j = i+1, j+4 {
			a := uint32(src[j+3])
			invA := 255 - a

			b0 := uint8((uint32(src[j])*a + uint32(dst[j])*invA + 127) / 255)
			b1 := uint8((uint32(src[j+1])*a + uint32(dst[j+1])*invA + 127) / 255)
			b2 := uint8((uint32(src[j+2])*a + uint32(dst[j+2])*invA + 127) / 255)

			out[j] = b0
			out[j+1] = b1
			out[j+2] = b2
			out[j+3] = 255

			h ^= uint64(b0)
			h *= prime
			h ^= uint64(b1)
			h *= prime
			h ^= uint64(b2)
			h *= prime
			h ^= 255
			h *= prime
		}
		hash = h
	}
	return hash
}
