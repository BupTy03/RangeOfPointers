#pragma once
#ifndef RANGE_OF_POINTERS_HPP
#define RANGE_OF_POINTERS_HPP

#include <type_traits>
#include <algorithm>
#include <cassert>
#include <iterator>


namespace range_of_ptrs {

	template <class Iter, typename = std::enable_if_t<std::is_pointer_v<typename std::iterator_traits<Iter>::value_type>>>
	struct raii_ptrs_range_wrapper {
	private:
		using PtrType = typename std::iterator_traits<Iter>::value_type;
		static_assert(std::is_pointer_v<PtrType>, "pointer type expected");

		using ValueType = std::remove_pointer_t<PtrType>;
	public:
		raii_ptrs_range_wrapper() = default;
		explicit raii_ptrs_range_wrapper(Iter dest) : first_{ dest }, last_{ dest } {}
		explicit raii_ptrs_range_wrapper(Iter first, Iter last) : first_{ first }, last_{ last } {}

		raii_ptrs_range_wrapper(const raii_ptrs_range_wrapper&) = delete;
		raii_ptrs_range_wrapper& operator=(const raii_ptrs_range_wrapper&) = delete;

		raii_ptrs_range_wrapper(raii_ptrs_range_wrapper&&) = delete;
		raii_ptrs_range_wrapper& operator=(raii_ptrs_range_wrapper&&) = delete;

		~raii_ptrs_range_wrapper() {
			for (; first_ != last_; ++first_) {
				delete(*first_);
			}
		}

		void update_range(Iter first, Iter last) { first_ = first; last_ = last; }

		template<typename Container>
		void update_range(const Container& cont) { update_range(std::begin(cont), std::end(cont)); }

		Iter release() { first_ = last_; return last_; }

	private:
		Iter first_;
		Iter last_;
	};


	template<typename Container, typename = std::enable_if_t<std::is_pointer_v<typename Container::value_type>>>
	struct raii_ptrs_container_wrapper {
		raii_ptrs_container_wrapper() = default;
		explicit raii_ptrs_container_wrapper(const Container& container) : pCont_{ &container } {}
		~raii_ptrs_container_wrapper() {
			if (pCont_ == nullptr) return;
			for (auto p : (*pCont_))
				delete p;
		}

		raii_ptrs_container_wrapper(const raii_ptrs_container_wrapper&) = delete;
		raii_ptrs_container_wrapper& operator=(const raii_ptrs_container_wrapper&) = delete;

		raii_ptrs_container_wrapper(raii_ptrs_container_wrapper&&) = delete;
		raii_ptrs_container_wrapper& operator=(raii_ptrs_container_wrapper&&) = delete;

		void change_container(const Container& container) { pCont_ = &container; }
		void release() { pCont_ = nullptr; }

	private:
		const Container* pCont_ = nullptr;
	};


	template<typename InIter, typename OutIter>
	OutIter Copy(InIter first, InIter last, OutIter dest)
	{
		for (; first != last; ++dest, ++first) {
			assert(*dest != nullptr);
			assert(*first != nullptr);
			*(*dest) = *(*first);
		}
		return dest;
	}

	template<typename InIter, typename SizeType, typename OutIter>
	OutIter CopyN(InIter first, SizeType count, OutIter dest)
	{
		if (count > 0) {
			while (true) {
				*(*dest) = *(*first);
				++dest;
				--count;
				if (count == 0) {
					break;
				}
				++first;
			}
		}
		return dest;
	}

	template<typename InIter, typename OutIter>
	OutIter CopyBackward(InIter first, InIter last, OutIter dest)
	{
		while (first != last) {
			*(*(--dest)) = *(*(--last));
		}
		return dest;
	}

	template<typename InIter, typename OutIter, typename Pred>
	OutIter CopyIf(InIter first, InIter last, OutIter dest, Pred pred)
	{
		for (; first != last; ++first) {
			assert(*dest != nullptr);
			assert(*first != nullptr);
			if (pred(*(*first))) {
				*(*dest) = *(*first);
				++dest;
			}
		}

		return dest;
	}


	template<typename InIter, typename OutIter>
	OutIter ReplaceCopy(InIter first, InIter last, OutIter dest)
	{
		using ValueType = std::decay_t<decltype(*(*dest))>;

		for (; first != last; ++dest, ++first) {
			assert(*dest != nullptr);
			assert(*first != nullptr);
			(*dest)->~ValueType();
			::new (*dest) ValueType(*(*first));
		}

		return dest;
	}

	template<typename InIter, typename OutIter, typename Pred>
	OutIter ReplaceCopyIf(InIter first, InIter last, OutIter dest, Pred pred)
	{
		using ValueType = std::decay_t<decltype(*(*dest))>;

		for (; first != last; ++dest, ++first) {
			assert(*dest != nullptr);
			assert(*first != nullptr);
			if (pred(*(*first))) {
				(*dest)->~ValueType();
				::new (*dest) ValueType(*(*first));
			}
		}

		return dest;
	}


	template<typename InIter, typename OutIter>
	OutIter Clone(InIter first, InIter last, OutIter dest)
	{
		for (; first != last; ++dest, ++first) {
			assert(*dest != nullptr);
			assert(*first != nullptr);
			*dest = (*first)->Clone();
		}
		return dest;
	}

	template<typename InIter, typename OutIter, typename Pred>
	OutIter CloneIf(InIter first, InIter last, OutIter dest, Pred pred)
	{
		for (; first != last; ++first) {
			assert(*dest != nullptr);
			assert(*first != nullptr);
			if (pred(*(*first))) {
				*dest = (*first)->Clone();
				++dest;
			}
		}
		return dest;
	}


	template<typename InIter, typename OutIter>
	OutIter ReplaceClone(InIter first, InIter last, OutIter dest)
	{
		for (; first != last; ++dest, ++first) {
			assert(*dest != nullptr);
			assert(*first != nullptr);
			delete(*dest);
			*dest = (*first)->Clone();
		}
		return dest;
	}

	template<typename InIter, typename OutIter, typename Pred>
	OutIter ReplaceCloneIf(InIter first, InIter last, OutIter dest, Pred pred)
	{
		for (; first != last; ++first) {
			assert(*dest != nullptr);
			assert(*first != nullptr);
			if (pred(*(*first))) {
				delete(*dest);
				*dest = (*first)->Clone();
				++dest;
			}
		}
		return dest;
	}


	template<typename ForwardIt, typename T>
	ForwardIt Remove(ForwardIt first, ForwardIt last, const T& value)
	{
		ForwardIt result = first;
		for (; first != last; ++first) {
			if (!(*(*first) == value)) {
				*result = *first;
				result++;
			}
			else {
				delete(*first);
				*first = nullptr;
			}
		}
		return result;
	}

	template<typename ForwardIt, typename T, typename Predicate>
	ForwardIt RemoveIf(ForwardIt first, ForwardIt last, const T& value, Predicate pred)
	{
		ForwardIt result = first;
		for (; first != last; ++first) {
			if (!pred(*(*first))) {
				*result = *first;
				result++;
			}
			else {
				delete(*first);
				*first = nullptr;
			}
		}
		return result;
	}


	template<typename ForwardIt>
	ForwardIt Unique(ForwardIt first, ForwardIt last)
	{
		if (first == last)
			return last;

		auto result = first;
		while (++first != last) {
			if (!(*(*result) == *(*first))) {
				++result;
				*result = *first;
			}
			else {
				delete(*first);
				*first = nullptr;
			}
		}
		++result;
		return result;
	}

	template<typename ForwardIt, typename BinaryPredicate>
	ForwardIt Unique(ForwardIt first, ForwardIt last, BinaryPredicate pred)
	{
		if (first == last)
			return last;

		auto result = first;
		while (++first != last) {
			if (!pred(*(*result), *(*first))) {
				++result;
				*result = *first;
			}
			else {
				delete(*first);
				*first = nullptr;
			}
		}
		++result;
		return result;
	}


	template<typename ToContainer, typename It,
		typename = std::enable_if_t<std::is_pointer_v<typename std::iterator_traits<It>::value_type>>,
		typename = std::enable_if_t<std::is_same_v<typename ToContainer::value_type, typename std::iterator_traits<It>::value_type>>
	>
	ToContainer DeepCopyOfRange(It first, It last)
	{
		using ValueType = std::remove_pointer_t<typename std::iterator_traits<It>::value_type>;

		ToContainer result;
		raii_ptrs_container_wrapper<ToContainer> backout{ result };

		result.reserve(std::distance(first, last));
		std::transform(first, last, std::back_inserter(result), [](auto pLeft) {
			assert(pLeft != nullptr);
			return new ValueType(*pLeft);
		});

		backout.release();
		return result;
	}

	template<typename FromContainer, typename ToContainer = FromContainer>
	ToContainer DeepCopy(const FromContainer& container)
	{ 
		return DeepCopyOfRange<ToContainer>(std::cbegin(container), std::cend(container)); 
	}


	template<typename UnaryFunctor>
	struct UnaryFunctorDerefAdapter {
		UnaryFunctorDerefAdapter() = default;
		explicit UnaryFunctorDerefAdapter(UnaryFunctor func) : func_{ func } {}

		template<typename T>
		bool operator()(T* ptr) const {
			assert(ptr != nullptr);
			return func_(*ptr);
		}
	private:
		UnaryFunctor func_;
	};

	template<typename BinaryFunctor>
	struct BinaryFunctorDerefAdapter {
		BinaryFunctorDerefAdapter() = default;
		explicit BinaryFunctorDerefAdapter(BinaryFunctor func) : func_{func} {}

		template<typename TypeLeft, typename TypeRight>
		bool operator()(TypeLeft* lhs, TypeRight rhs) const {
			assert(lhs != nullptr);
			return func_(*lhs, std::forward<TypeRight>(rhs));
		}

	private:
		BinaryFunctor func_;
	};

	template<typename BinaryFunctor>
	struct BinaryFunctorDerefPtrsAdapter {
		BinaryFunctorDerefPtrsAdapter() = default;
		explicit BinaryFunctorDerefPtrsAdapter(BinaryFunctor func) : func_{ func } {}

		template<typename T>
		bool operator()(T* lhs, T* rhs) const {
			assert(lhs != nullptr);
			assert(rhs != nullptr);
			return func_(*lhs, *rhs);
		}

	private:
		BinaryFunctor func_;
	};
}



#endif // !RANGE_OF_POINTERS_HPP
