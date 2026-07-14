package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	dataLen := pixels * 4
	out := make([]byte, dataLen)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		
		// Process pixels in a single pass to maximize cache efficiency
		for i := 0; i < pixels; i++ {
			base := i << 2
			sr := uint32(src[base])
			sg := uint32(src[base+1])
			sb := uint32(src[base+2])
			sa := uint32(src[base+3])
			
			dr := uint32(dst[base])
			dg := uint32(dst[base+1])
			db := uint32(dst[base+2])

			// Optimized blend: (s*a + d*(255-a) + 127) / 255
			// Fast division by 255: (v + 1 + (v >> 8)) >> 8
			invA := 255 - sa
			
			vr := sr*sa + dr*invA + 127
			r := uint8((vr + 1 + (vr >> 8)) >> 8)
			
			vg := sg*sa + dg*invA + 127
			g := uint8((vg + 1 + (vg >> 8)) >> 8)
			
			vb := sb*sa + db*invA + 127
			b := uint8((vb + 1 + (vb >> 8)) >> 8)

			// Update output buffer
			out[base] = r
			out[base+1] = g
			out[base+2] = b
			out[base+3] = 255

			// Inline FNV-1a hash calculation for the 4 bytes
			hash ^= uint64(r)
			hash *= 1099511628211
			hash ^= uint64(g)
			hash *= 1099511628211
			hash ^= uint64(b)
			hash *= 1099511628211
			hash ^= uint64(255)
			hash *= 1099511628211
		}
	}
	return hash
}
