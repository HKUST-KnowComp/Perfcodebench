package main

func run(src, dst []byte, width, height, iters int) uint64 {
	// We compute the checksum directly during blending, avoiding building an output slice.
	// This preserves exact byte order: R, G, B, A for each pixel, with A = 255.
	outLen := width * height * 4

	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for base := 0; base < outLen; base += 4 {
			// Load alpha and precompute inverse alpha
			a := uint32(src[base+3])
			ia := uint32(255) - a

			// Load source and destination RGB channels
			sr := uint32(src[base])
			sg := uint32(src[base+1])
			sb := uint32(src[base+2])

			dr := uint32(dst[base])
			dg := uint32(dst[base+1])
			db := uint32(dst[base+2])

			// Blend each channel with rounding: (s*a + d*(255-a) + 127) / 255
			r := uint8((sr*a + dr*ia + 127) / 255)
			g := uint8((sg*a + dg*ia + 127) / 255)
			b := uint8((sb*a + db*ia + 127) / 255)

			// Update FNV-1 hash with the blended bytes in RGBA order (A = 255)
			h ^= uint64(r)
			h *= fnvPrime
			h ^= uint64(g)
			h *= fnvPrime
			h ^= uint64(b)
			h *= fnvPrime
			h ^= 255 // alpha is 255
			h *= fnvPrime
		}
		hash = h
	}
	return hash
}
