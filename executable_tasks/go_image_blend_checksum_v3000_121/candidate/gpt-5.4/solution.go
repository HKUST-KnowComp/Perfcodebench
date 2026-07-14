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
		si := 0
		for si < n {
			a := uint32(src[si+3])
			ia := 255 - a

			v := byte((uint32(src[si])*a + uint32(dst[si])*ia + 127) / 255)
			out[si] = v
			h ^= uint64(v)
			h *= prime

			v = byte((uint32(src[si+1])*a + uint32(dst[si+1])*ia + 127) / 255)
			out[si+1] = v
			h ^= uint64(v)
			h *= prime

			v = byte((uint32(src[si+2])*a + uint32(dst[si+2])*ia + 127) / 255)
			out[si+2] = v
			h ^= uint64(v)
			h *= prime

			out[si+3] = 255
			h ^= 255
			h *= prime

			si += 4
		}
		hash = h
	}
	return hash
}
