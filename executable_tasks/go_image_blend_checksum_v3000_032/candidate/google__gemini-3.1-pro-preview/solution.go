package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	dataLen := pixels * 4
	out := make([]byte, dataLen)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i << 2
			sr := uint32(src[base])
			sg := uint32(src[base+1])
			sb := uint32(src[base+2])
			a := uint32(src[base+3])
			invA := 255 - a

			dr := uint32(dst[base])
			dg := uint32(dst[base+1])
			db := uint32(dst[base+2])

			// Fast rounding division by 255: (v + 127) / 255 == ((v + 128) * 257) >> 16
			vr := sr*a + dr*invA + 128
			vg := sg*a + dg*invA + 128
			vb := sb*a + db*invA + 128

			out[base] = uint8((vr * 257) >> 16)
			out[base+1] = uint8((vg * 257) >> 16)
			out[base+2] = uint8((vb * 257) >> 16)
			out[base+3] = 255
		}

		// FNV-1a checksum
		hash = 1469598103934665603
		const prime uint64 = 1099511628211
		
		// Unroll checksum loop to reduce overhead
		i := 0
		for ; i <= dataLen-4; i += 4 {
			hash ^= uint64(out[i])
			hash *= prime
			hash ^= uint64(out[i+1])
			hash *= prime
			hash ^= uint64(out[i+2])
			hash *= prime
			hash ^= uint64(out[i+3])
			hash *= prime
		}
		for ; i < dataLen; i++ {
			hash ^= uint64(out[i])
			hash *= prime
		}
	}
	return hash
}