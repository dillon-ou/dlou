#pragma once

namespace dlou {

template <class Type, class Del, Type InvalidVal = Type{} >
class unique_handle
{
public:
	using element_type = Type;
	using deleter_type = Del;
	static constexpr element_type invalid_value = InvalidVal;

	unique_handle() = default;
	explicit unique_handle(element_type v) : h_(v) {}
	unique_handle(const unique_handle&) = delete;
	unique_handle(unique_handle&& x) : h_(x.release()) {}
	~unique_handle() {
		if (invalid_value != h_)
			deleter_type{}(h_);
	}

	unique_handle& operator =(const unique_handle&) = delete;
	unique_handle& operator =(unique_handle&& x) {
		reset(x.release());
		return *this;
	}

	explicit operator bool() const { return invalid_value != h_; }
	bool operator !() const { return invalid_value == h_; }
	element_type operator *() const { return h_; }

	element_type release() { auto ret = h_; h_ = invalid_value; return ret; }

	void reset(element_type v = invalid_value) {
		this->~unique_handle();
		h_ = v;
	}

	void swap(unique_handle& x) {
		auto tmp = x.h_;
		x.h_ = h_;
		h_ = tmp;
	}

private:
	element_type h_ = invalid_value;
};

} // namespace dlou
