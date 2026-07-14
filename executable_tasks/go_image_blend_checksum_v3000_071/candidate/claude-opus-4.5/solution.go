package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i << 2
			outBase := base

			a := uint32(src[base+3])
			invA := 255 - a

			sr := uint32(src[base])
			sg := uint32(src[base+1])
			sb := uint32(src[base+2])

			dr := uint32(dst[base])
			dg := uint32(dst[base+1])
			db := uint32(dst[base+2])

			out[outBase] = uint8((sr*a + dr*invA + 127) / 255)
			out[outBase+1] = uint8((sg*a + dg*invA + 127) / 255)
			out[outBase+2] = uint8((sb*a + db*invA + 127) / 255)
			out[outBase+3] = 255
		}

		h := uint64(1469598103934665603)
		for j := 0; j < outLen; j++ {
			h ^= uint64(out[j])
			h *= 1099511628211
		}
		hash = h
	}
	return hash
}