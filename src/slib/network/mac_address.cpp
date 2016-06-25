#include "../../../inc/slib/network/mac_address.h"

SLIB_NETWORK_NAMESPACE_BEGIN

const sl_uint8 MacAddress::_zero[6] = { 0 };
const sl_uint8 MacAddress::_broadcast[6] = { 255, 255, 255, 255, 255, 255 };

MacAddress::MacAddress(const MacAddress& other) = default;

MacAddress::MacAddress(const sl_uint8* _m)
{
	m[0] = _m[0];
	m[1] = _m[1];
	m[2] = _m[2];
	m[3] = _m[3];
	m[4] = _m[4];
	m[5] = _m[5];
}

MacAddress::MacAddress(sl_uint8 m0, sl_uint8 m1, sl_uint8 m2, sl_uint8 m3, sl_uint8 m4, sl_uint8 m5)
{
	m[0] = m0;
	m[1] = m1;
	m[2] = m2;
	m[3] = m3;
	m[4] = m4;
	m[5] = m5;
}

MacAddress::MacAddress(const String& address)
{
	setString(address);
}

void MacAddress::setZero()
{
	m[0] = 0;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;
	m[4] = 0;
	m[5] = 0;
}

void MacAddress::setBroadcast()
{
	m[0] = 255;
	m[1] = 255;
	m[2] = 255;
	m[3] = 255;
	m[4] = 255;
	m[5] = 255;
}

void MacAddress::makeMulticast(const IPv4Address& addrMulticast)
{
	m[0] = 1;
	m[1] = 0;
	m[2] = 0x5e;
	m[3] = addrMulticast.b & 0x7F;
	m[4] = addrMulticast.c;
	m[5] = addrMulticast.d;
}

void MacAddress::makeMulticast(const IPv6Address& addrMulticast)
{
	m[0] = 0x33;
	m[1] = 0x33;
	m[2] = addrMulticast.m[12];
	m[3] = addrMulticast.m[13];
	m[4] = addrMulticast.m[14];
	m[5] = addrMulticast.m[15];
}

void MacAddress::getBytes(sl_uint8* _m) const
{
	_m[0] = m[0];
	_m[1] = m[1];
	_m[2] = m[2];
	_m[3] = m[3];
	_m[4] = m[4];
	_m[5] = m[5];
}

void MacAddress::setBytes(const sl_uint8* _m)
{
	m[0] = _m[0];
	m[1] = _m[1];
	m[2] = _m[2];
	m[3] = _m[3];
	m[4] = _m[4];
	m[5] = _m[5];
}

int MacAddress::compare(const MacAddress& other) const
{
	return Base::compareMemory(m, other.m, 6);
}

sl_uint32 MacAddress::hashCode() const
{
	return sl_hash(m, 6);
}

String MacAddress::toString(sl_char8 sep) const
{
	String ret;
	for (int i = 0; i < 6; i++) {
		if (i) {
			ret += String(sep, 1);
		}
		ret += String::fromUint32(m[i], 16, 2, sl_true);
	}
	return ret;
}

sl_bool MacAddress::setString(const String& str)
{
	if (parse(str)) {
		return sl_true;
	} else {
		setZero();
		return sl_false;
	}
}

template <class CT>
SLIB_INLINE sl_reg _MacAddress_parse(MacAddress* obj, const CT* sz, sl_size i, sl_size n, sl_char8 sep)
{
	sl_char8 sep1 = sep;
	sl_char8 sep2 = sep;
	if (sep == 0) {
		sep1 = '-';
		sep2 = ':';
	}
	int v[6];
	for (int k = 0; k < 6; k++) {
		int t = 0;
		int s = 0;
		for (; i < n; i++) {
			int h = sz[i];
			if (h >= '0' && h <= '9') {
				s = (s << 4) | (h - '0');
				if (s > 255) {
					return SLIB_PARSE_ERROR;
				}
				t++;
			} else if (h >= 'A' && h <= 'F') {
				s = (s << 4) | (h - 'A' + 10);
				if (s > 255) {
					return SLIB_PARSE_ERROR;
				}
				t++;
			} else if (h >= 'a' && h <= 'f') {
				s = (s << 4) | (h - 'a' + 10);
				if (s > 255) {
					return SLIB_PARSE_ERROR;
				}
				t++;
			} else {
				break;
			}
		}
		if (k < 5) {
			if (i >= n || (sz[i] != sep1 && sz[i] != sep2)) {
				return SLIB_PARSE_ERROR;
			}
			i++;
		}
		if (t == 0) {
			return SLIB_PARSE_ERROR;
		}
		v[k] = s;
	}
	if (obj) {
		obj->m[0] = (sl_uint8)(v[0]);
		obj->m[1] = (sl_uint8)(v[1]);
		obj->m[2] = (sl_uint8)(v[2]);
		obj->m[3] = (sl_uint8)(v[3]);
		obj->m[4] = (sl_uint8)(v[4]);
		obj->m[5] = (sl_uint8)(v[5]);
	}
	return i;
}

sl_reg MacAddress::parse(MacAddress* out, const sl_char8* sz, sl_size posBegin, sl_size len, sl_char8 sep)
{
	return _MacAddress_parse(out, sz, posBegin, len, sep);
}

sl_reg MacAddress::parse(MacAddress* out, const sl_char16* sz, sl_size posBegin, sl_size len, sl_char8 sep)
{
	return _MacAddress_parse(out, sz, posBegin, len, sep);
}

sl_bool MacAddress::parse(const String& s, sl_char8 sep)
{
	sl_size n = s.getLength();
	if (n == 0) {
		return sl_false;
	}
	return _MacAddress_parse(this, s.getData(), 0, n, sep) == n;
}

MacAddress& MacAddress::operator=(const MacAddress& other) = default;

MacAddress& MacAddress::operator=(const String& address)
{
	setString(address);
	return *this;
}

template <>
int Compare<MacAddress>::compare(const MacAddress& a, const MacAddress& b)
{
	return a.compare(b);
}

template <>
sl_bool Compare<MacAddress>::equals(const MacAddress& a, const MacAddress& b)
{
	return a == b;
}

template <>
sl_uint32 Hash<MacAddress>::hash(const MacAddress& a)
{
	return a.hashCode();
}

SLIB_NETWORK_NAMESPACE_END
