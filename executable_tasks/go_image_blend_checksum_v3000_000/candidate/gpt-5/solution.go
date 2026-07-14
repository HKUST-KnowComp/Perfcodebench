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
	total := pixels * 4
	var result uint64
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for iter := 0; iter < iters; iter++ {
		// Compute FNV-1a over blended output without materializing it.
		h := offset
		for i := 0; i < total; i += 4 {
			// Load alpha once per pixel
			aByte := src[i+3]
			a := uint32(aByte)
			inv := uint32(255) - a

			// R
			r := (uint32(src[i])*a + uint32(dst[i])*inv + 127) / 255
			h ^= uint64(uint8(r))
			h *= prime
			// G
			g := (uint32(src[i+1])*a + uint32(dst[i+1])*inv + 127) / 255
			h ^= uint64(uint8(g))
			h *= prime
			// B
			b := (uint32(src[i+2])*a + uint32(dst[i+2])*inv + 127) / 255
			h ^= uint64(uint8(b))
			h *= prime
			// A is constant 255 in output
			h ^= uint64(255)
			h *= prime
		}
		result = h
	}
	return result
}
