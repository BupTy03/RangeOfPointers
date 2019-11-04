#include "RangeOfPointers.hpp"
#include "TestObject.hpp"

#include <vector>


void print(const std::vector<my::TestObject*>& vec)
{
	std::cout << "==================================\n";
	for (auto pObj : vec) {
		std::cout << *pObj << '\n';
	}
	std::cout << "==================================\n";
}


int main()
{
	using namespace my;
	using namespace range_of_ptrs;

	{
		std::vector<TestObject*> pVec;
		raii_ptrs_container_wrapper<std::vector<TestObject*>> wrapper{ pVec };

		pVec.push_back(new TestObject(1));
		for (int i = 0; i < 10; ++i) {
			pVec.push_back(new TestObject(i));
		}
		pVec.push_back(new TestObject(1));

		pVec.push_back(new TestObject(9));
		pVec.push_back(new TestObject(9));
		pVec.push_back(new TestObject(9));

		std::cout << "\nBefore\n";
		print(pVec);

		std::cout << "\nAfter sort\n";
		std::sort(std::begin(pVec), std::end(pVec), BinaryFunctorDerefPtrsAdapter<std::less<>>());
		print(pVec);

		std::cout << "\nAfter Erase-Unique\n";
		pVec.erase(Unique(std::begin(pVec), std::end(pVec), std::equal_to<>()), std::end(pVec));
		print(pVec);
	}

	return 0;
}