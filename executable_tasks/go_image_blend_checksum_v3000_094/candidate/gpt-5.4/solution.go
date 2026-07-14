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
		for base := 0; base < n; base += 4 {
			a := uint32(src[base+3])
			ia := 255 - a

			v0 := byte((uint32(src[base])*a + uint32(dst[base])*ia + 127) / 255)
			v1 := byte((uint32(src[base+1])*a + uint32(dst[base+1])*ia + 127) / 255)
			v2 := byte((uint32(src[base+2])*a + uint32(dst[base+2])*ia + 127) / 255)

			out[base] = v0
			out[base+1] = v1
			out[base+2] = v2
			out[base+3] = 255

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
