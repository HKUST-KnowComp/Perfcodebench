package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	dataLen := pixels * 4
	out := make([]byte, dataLen)
	var hash uint64

	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for i := 0; i < pixels; i++ {
			base := i << 2
			sr := uint32(src[base])
			sg := uint32(src[base+1])
			sb := uint32(src[base+2])
			a := uint32(src[base+3])
			invA := 255 - a

			// Optimized blend: (s*a + d*(255-a) + 127) / 255
			// Using (t * 0x8081) >> 23 as a fast exact replacement for t / 255 for t <= 65535
			r := uint8(((sr*a + uint32(dst[base])*invA + 127) * 0x8081) >> 23)
			g := uint8(((sg*a + uint32(dst[base+1])*invA + 127) * 0x8081) >> 23)
			b := uint8(((sb*a + uint32(dst[base+2])*invA + 127) * 0x8081) >> 23)

			out[base] = r
			out[base+1] = g
			out[base+2] = b
			out[base+3] = 255

			// Inline FNV-1a checksum calculation
			h = (h ^ uint64(r)) * fnvPrime
			h = (h ^ uint64(g)) * fnvPrime
			h = (h ^ uint64(b)) * fnvPrime
			h = (h ^ uint64(255)) * fnvPrime
		}
		hash = h
	}
	return hash
}
