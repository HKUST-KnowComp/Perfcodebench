package main

// Precomputed blend table: blendTable[a][s] = (s*a + 127) / 255
// This allows us to compute blend as: blendTable[a][s] + blendTable[255-a][d]
var blendTable [256][256]uint8

func init() {
	for a := 0; a < 256; a++ {
		for s := 0; s < 256; s++ {
			blendTable[a][s] = uint8((uint32(s)*uint32(a) + 127) / 255)
		}
	}
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			ia := 255 - a
			tableA := &blendTable[a]
			tableIA := &blendTable[ia]

			out[base] = tableA[src[base]] + tableIA[dst[base]]
			out[base+1] = tableA[src[base+1]] + tableIA[dst[base+1]]
			out[base+2] = tableA[src[base+2]] + tableIA[dst[base+2]]
			out[base+3] = 255
		}

		// FNV-1a hash with unrolled loop
		hash = 1469598103934665603
		i := 0
		for ; i+7 < outLen; i += 8 {
			hash ^= uint64(out[i])
			hash *= 1099511628211
			hash ^= uint64(out[i+1])
			hash *= 1099511628211
			hash ^= uint64(out[i+2])
			hash *= 1099511628211
			hash ^= uint64(out[i+3])
			hash *= 1099511628211
			hash ^= uint64(out[i+4])
			hash *= 1099511628211
			hash ^= uint64(out[i+5])
			hash *= 1099511628211
			hash ^= uint64(out[i+6])
			hash *= 1099511628211
			hash ^= uint64(out[i+7])
			hash *= 1099511628211
		}
		for ; i < outLen; i++ {
			hash ^= uint64(out[i])
			hash *= 1099511628211
		}
	}
	return hash
}