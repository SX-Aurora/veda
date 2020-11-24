#pragma once

struct VEDAptr {
	typedef uint32_t Idx;
	typedef uint32_t Offset;

private:
	union {
		struct {
			Offset offset;
			union {
				struct {
					uint8_t __reserved__[3];
					uint8_t id;
				} device;
				Idx id;
			} idx;
		} mask;
		uint64_t vptr;
	} m_ptr;

public:
	inline VEDAptr(const VEDAdevice device, const uint32_t id) {
		m_ptr.vptr = 0;
		m_ptr.mask.idx.id = id;
		m_ptr.mask.idx.device.id = device;
	}

	inline VEDAptr(const VEDAdeviceptr ptr) {
		m_ptr.vptr = ptr;
	}

	inline	VEDAdevice	device		(void) const	{	return m_ptr.mask.idx.device.id;	}
	inline	Idx		idx		(void) const	{	return m_ptr.mask.idx.id;		}
	inline	Offset		offset		(void) const	{	return m_ptr.mask.offset;		}
	inline	VEDAdeviceptr	vptr		(void) const	{	return m_ptr.vptr;			}
	inline	operator	VEDAdeviceptr	(void) const	{	return m_ptr.vptr;			}
	inline	VEDAdeviceptr	base		(void) const	{	return VEDAptr(device(), idx()).vptr();	}
};
