#pragma once

#include "integer.hpp"

#include <cstddef>
#include <cstdint>

namespace dlou {

static constexpr size_t none = ~size_t(0);

// full binary tree
template<uint8_t Level>
class buddy
{
public:
	static constexpr uint8_t max_level = Level;
	static constexpr size_t max_size = size_t(1) << max_level;

private:
	static constexpr size_t node_count = max_size * 2 - 1;

public:
	buddy() {
		uint8_t val = max_level + 1;
		size_t idx = 0;
		for (size_t i = 1; i <= max_size; i <<= 1) {
			for (size_t j = 0; j < i; ++j)
				node_[idx++] = val;

			--val;
		}
	}

	bool empty() const {
		return max_level + 1 == node_[0];
	}

	bool full() const {
		return !node_[0];
	}

	// return buffer offset
	size_t allocate(size_t siz) {
		return allocate_by_pow2(to_exp(siz));
	}

	// same allocate(pow2(exp))
	size_t allocate_by_pow2(uint8_t exp) {
		if (exp >= node_[0])
			return none;

		const auto arr = btree();
		static constexpr auto beg = root();
		auto idx = beg; // index

		uint8_t lval, rval;
		for (uint8_t level = max_level; level != exp; --level) {
			auto left = to_left(idx);
			auto right = to_right(idx);
			lval = arr[left];

			if (exp < lval) {
				rval = arr[left];
				if (exp < rval)
					idx = (lval <= rval) ? left : right;
				else
					idx = left;
			}
			else
				idx = right;
		}

		arr[idx] = lval = 0;

		size_t pos = get_offset(exp, idx);

		// lval is current value
		// rval is buddy value
		while (beg != idx) {
			rval = arr[to_buddy(idx)];
			idx = to_parent(idx);
			if (lval < rval)
				lval = rval;
			arr[idx] = lval;
		}

		return pos;
	}

	void release(size_t pos) {
#ifdef DLOU_CHECK_ARGS
		if (max_size <= pos)
			return;
#endif

		const auto arr = btree();
		static constexpr auto beg = root();
		static constexpr size_t offset = get_leftmost(0);
		auto idx = offset + pos;
		uint8_t cval = 1; // current value

		while(arr[idx]) {
#ifdef DLOU_CHECK_ARGS
			// not found
			if (beg == idx)
				return;
#endif
			idx = to_parent(idx);
			++cval;
		}

		arr[idx] = cval;

		uint8_t bval; // buddy value

		while (beg != idx) {
			bval = arr[to_buddy(idx)];
			idx = to_parent(idx);
			// cval is max value for this level
			if (cval != bval) {
				arr[idx] = cval;

				while (idx) {
					bval = arr[to_buddy(idx)];
					idx = to_parent(idx);
					if (cval < bval)
						return;
					arr[idx] = cval;
				}
				return;
			}
			arr[idx] = ++cval;
		}
	}

	void release(size_t pos, size_t siz) {
#ifdef DLOU_CHECK_ARGS
		if (!base2::ispow(siz))
			return;
#endif
		return release_by_pow2(pos, base2::log(siz));
	}

	void release_by_pow2(size_t pos, uint8_t exp) {
#ifdef DLOU_CHECK_ARGS
		if (max_size <= pos
			|| exp > max_level
			|| ~(~size_t(0) << exp) & pos)
			return;
#endif

		const auto arr = btree();
		static constexpr auto beg = root();
		auto idx = get_leftmost(exp) + (pos >> exp);
		uint8_t cval = exp + 1; // current value

#ifdef DLOU_CHECK_ARGS
		if ((0 != arr[idx])
			|| (0 != exp
				&& 0 == arr[to_left(idx)]))
			return;
#endif

		arr[idx] = cval;

		uint8_t bval; // buddy value

		while (beg != idx) {
			bval = arr[to_buddy(idx)];
			idx = to_parent(idx);
			// cval is max value for level
			if (cval != bval) {
				arr[idx] = cval;

				while (idx) {
					bval = arr[to_buddy(idx)];
					idx = to_parent(idx);
					if (cval < bval)
						return;
					arr[idx] = cval;
				}
				return;
			}
			arr[idx] = ++cval;
		}
	}

protected:
	static constexpr uint8_t to_exp(size_t siz) {
		return
#ifdef DLOU_CHECK_ARGS
		(!siz) ? uint8_t(0) :
#endif
			(base2::log(siz) + !base2::ispow(siz));
	}

	// 1-based indexing
	static constexpr size_t start = 1;
	constexpr uint8_t* btree() { return node_ - start; }
	static constexpr size_t root() { return start; }
	static constexpr size_t to_left(size_t idx) { return idx << 1; }
	static constexpr size_t to_right(size_t idx) { return (idx << 1) | size_t(1); }
	static constexpr size_t to_parent(size_t idx) { return idx >> 1; }
	static constexpr size_t to_buddy(size_t idx) { return idx ^ size_t(1); }

	static constexpr size_t get_leftmost(uint8_t exp) {
		// width (node count at target depth)
		//  = 1 << (max_level - exp)
		// leftmost (leftmost node at target depth)
		//  = (2 * ({width} / 2) - 1) + 1
		//  = {width}
		return size_t(1) << (max_level - exp);
	}

	static constexpr size_t get_offset(uint8_t exp, size_t idx) {
		// index = idx - 1
		// size (return node size)
		//  = 1 << exp
		// width (node count at target depth)
		//  = max_size / {size}
		// leftmost (leftmost node at target depth)
		//  = 2 * ({width} / 2) - 1
		//  = max_size / {size} - 1
		// position (node position are mapped to leaf layers)
		//  = {size} * ({index} - {leftmost})
		//  = {size} * (idx - (max_size / {size}))
		//  = {size} * idx - max_size
		return (idx << exp) - max_size;
	}

private:
	uint8_t node_[node_count];
};


template<size_t MinSize, size_t MaxSize>
class simple_buddy
{
	static_assert(MaxSize >= MinSize, "Check simple_buddy template args");

	static constexpr size_t unit(size_t siz) {
		return (siz + min_size - 1) / min_size;
	}
	static constexpr uint8_t to_exp(size_t siz) {
		return base2::log(siz) + !base2::ispow(siz);
	}
public:
	static constexpr size_t min_size = MinSize ? MinSize : 1;
	static constexpr auto max_level = to_exp(unit(MaxSize));
	static constexpr size_t max_size = (1 << max_level) * min_size;

public:
	bool empty() const { return base_.empty(); }
	bool full() const { return base_.full(); }

	size_t allocate(size_t siz) {
		auto ret = base_.allocate(unit(siz));
		return ret == none ? none : (ret * min_size);
	}

	void release(size_t pos) {
		size_t new_pos = pos / min_size;
#ifdef DLOU_CHECK_ARGS
		if (pos != new_pos * min_size)
			return;
#endif
		base_.release(new_pos);
	}

	void release(size_t pos, size_t siz) {
		size_t new_pos = pos / min_size;
		size_t new_siz = siz / min_size;
#ifdef DLOU_CHECK_ARGS
		if (pos != new_pos * min_size
			|| siz != new_siz * min_size)
			return;
#endif
		base_.release(new_pos, new_siz);
	}

private:
	buddy<max_level> base_;
};

} // namespace dlou
