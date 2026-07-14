package main

const fnv64Offset = 1469598103934665603
const fnv64Prime = 1099511628211

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = fnv64Offset
	for _, b := range data {
		hash ^= uint64(b)
		hash *= fnv64Prime
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	end := pixels * 4

	if end == 0 {
		var hash uint64
		for iter := 0; iter < iters; iter++ {
			hash = fnv64Offset
		}
		return hash
	}

	_ = src[end-1]
	_ = dst[end-1]

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := uint64(fnv64Offset)
		for base := 0; base < end; base += 4 {
			a := src[base+3]
			inv := uint32(255 - a)
			sa := uint32(a)

			v0 := uint8((uint32(src[base+0])*sa + uint32(dst[base+0])*inv + 127) / 255)
			h ^= uint64(v0)
			h *= fnv64Prime

			v1 := uint8((uint32(src[base+1])*sa + uint32(dst[base+1])*inv + 127) / 255)
			h ^= uint64(v1)
			h *= fnv64Prime

			v2 := uint8((uint32(src[base+2])*sa + uint32(dst[base+2])*inv + 127) / 255)
			h ^= uint64(v2)
			h *= fnv64Prime

			h ^= 255
			h *= fnv64Prime
		}
		hash = h
	}
	return hash
}
