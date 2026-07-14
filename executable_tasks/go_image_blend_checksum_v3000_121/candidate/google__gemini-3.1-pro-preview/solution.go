package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	dataLen := pixels * 4
	out := make([]byte, dataLen)
	
	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = fnvOffset
		for i := 0; i < dataLen; i += 4 {
			sr := uint32(src[i])
			sg := uint32(src[i+1])
			sb := uint32(src[i+2])
			sa := uint32(src[i+3])

			dr := uint32(dst[i])
			dg := uint32(dst[i+1])
			db := uint32(dst[i+2])

			// Optimized blend: (s*a + d*(255-a) + 127) / 255
			// Equivalent to: (a*(s-d) + 255*d + 127) / 255
			// Fast division by 255: (v + 128 + (v >> 8)) >> 8
			
			vr := sa*(sr-dr) + 255*dr
			tmpR := vr + 128
			r := uint8((tmpR + (tmpR >> 8)) >> 8)

			vg := sa*(sg-dg) + 255*dg
			tmpG := vg + 128
			g := uint8((tmpG + (tmpG >> 8)) >> 8)

			vb := sa*(sb-db) + 255*db
			tmpB := vb + 128
			b := uint8((tmpB + (tmpB >> 8)) >> 8)

			// Store result
			out[i] = r
			out[i+1] = g
			out[i+2] = b
			out[i+3] = 255

			// Inline FNV-1a checksum
			hash = (hash ^ uint64(r)) * fnvPrime
			hash = (hash ^ uint64(g)) * fnvPrime
			hash = (hash ^ uint64(b)) * fnvPrime
			hash = (hash ^ uint64(255)) * fnvPrime
		}
	}
	return hash
}