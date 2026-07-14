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
	n := pixels * 4
	out := make([]byte, n)

	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := offset
		for i := 0; i < n; i += 4 {
			a := uint32(src[i+3])
			invA := 255 - a

			v0 := uint8((uint32(src[i])*a + uint32(dst[i])*invA + 127) / 255)
			v1 := uint8((uint32(src[i+1])*a + uint32(dst[i+1])*invA + 127) / 255)
			v2 := uint8((uint32(src[i+2])*a + uint32(dst[i+2])*invA + 127) / 255)

			out[i] = v0
			out[i+1] = v1
			out[i+2] = v2
			out[i+3] = 255

			h ^= uint64(v0)
			h *= prime
			h ^= uint64(v1)
			h *= prime
			h ^= uint64(v2)
			h *= prime
			h ^= 255
			h *= prime
		}
		hash = h
	}
	return hash
}
