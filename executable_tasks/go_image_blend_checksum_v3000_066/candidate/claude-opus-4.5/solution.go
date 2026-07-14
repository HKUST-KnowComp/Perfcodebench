package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := uint32(src[base+3])
			na := 255 - a
			out[base] = uint8((uint32(src[base])*a + uint32(dst[base])*na + 127) / 255)
			out[base+1] = uint8((uint32(src[base+1])*a + uint32(dst[base+1])*na + 127) / 255)
			out[base+2] = uint8((uint32(src[base+2])*a + uint32(dst[base+2])*na + 127) / 255)
			out[base+3] = 255
		}

		// FNV-1a hash
		h := uint64(1469598103934665603)
		for j := 0; j < outLen; j++ {
			h ^= uint64(out[j])
			h *= 1099511628211
		}
		hash = h
	}
	return hash
}