package main

import "unsafe"

// exact 255 divisor using multiply-shift (Knuth)
const m = 0x8080808080808081

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, pixels*4)
	var hash uint64

	// 4-byte stride for both src and dst
	sStride := pixels * 4
	dStride := pixels * 4

	for iter := 0; iter < iters; iter++ {
		s := src
		d := dst
		o := out

		for i := 0; i < pixels; i++ {
			a := s[3]
			aInv := uint32(255 - a)

			// Red
			v := uint32(s[0])*uint32(a) + uint32(d[0])*aInv + 127
			o[0] = uint8((v * 0x8081) >> 23)

			// Green
			v = uint32(s[1])*uint32(a) + uint32(d[1])*aInv + 127
			o[1] = uint8((v * 0x8081) >> 23)

			// Blue
			v = uint32(s[2])*uint32(a) + uint32(d[2])*aInv + 127
			o[2] = uint8((v * 0x8081) >> 23)

			o[3] = 255

			s = s[4:]
			d = d[4:]
			o = o[4:]
		}

		// compute checksum over the whole buffer
		var h uint64 = 1469598103934665603
		b := out
		for len(b) >= 8 {
			h ^= *(*uint64)(unsafe.Pointer(&b[0]))
			h *= 1099511628211
			b = b[8:]
		}
		for _, v := range b {
			h ^= uint64(v)
			h *= 1099511628211
		}
		hash = h
	}
	return hash
}
