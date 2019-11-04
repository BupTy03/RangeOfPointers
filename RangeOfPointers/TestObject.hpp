#pragma once
#ifndef TEST_OBJECT_HPP
#define TEST_OBJECT_HPP

#include <iostream>
#include <string>
#include <vector>


namespace my
{
	struct TestObject {
		static int livingObjectsCount() { return living_objects_count_; }

		TestObject() : id_{ ++counter_ }
		{
			std::cout << "[object " << id_ << "] " << "TestObject::TestObject() (default constructor)" << std::endl;
			status_ = "constructed with default constructor";
			history_.push_back(status_);
			++living_objects_count_;
		}

		TestObject(int v) : id_{ ++counter_ }, val_{ v }
		{
			std::cout << "[object " << id_ << "] " << "TestObject::TestObject() (constructor with value)" << std::endl;
			status_ = "constructed with value ";
			status_ += std::to_string(v);
			history_.push_back(status_);
			++living_objects_count_;
		}

		TestObject(const TestObject& other) : id_{ ++counter_ }, parent_id_{ other.id_ }, val_{ other.val_ }
		{
			std::cout << "[object " << id_ << "] " << "TestObject::TestObject(const TestObject& other) (copy constructor)" << std::endl;
			status_ = "constructed with copy constructor from object with id: ";
			status_ += std::to_string(other.id_);
			history_.push_back(status_);
			++living_objects_count_;
		}
		TestObject& operator=(const TestObject& other)
		{
			if (this == &other) return *this;
			std::cout << "[object " << id_ << "] " << "TestObject& TestObject::operator=(const TestObject& other) (copy assignment operator)" << std::endl;
			status_ = "parameters of this object is assigned with parameters of object with id: ";
			status_ += std::to_string(other.id_);
			history_.push_back(status_);
			val_ = other.val_;
			return *this;
		}

		TestObject(TestObject&& other) noexcept : id_{ ++counter_ }, parent_id_{ other.id_ }
		{
			std::cout << "[object " << id_ << "] " << "TestObject::TestObject(TestObject&& other) (move constructor)" << std::endl;
			status_ = "constructed with move constructor from object with id: ";
			status_ += std::to_string(other.id_);
			other.status_ = "is moved to new object with id: ";
			other.status_ += std::to_string(id_);
			other.history_.push_back(other.status_);
			val_ = other.val_;
			other.val_ = 0;
			history_.push_back(status_);
			++living_objects_count_;
		}
		TestObject& operator=(TestObject&& other) noexcept
		{
			if (this == &other) return *this;

			std::cout << "TestObject& TestObject::operator=(TestObject&& other) (move assignment operator)" << std::endl;
			other.status_ = "this object is moved to object with id: ";
			other.status_ += std::to_string(this->id_);
			other.history_.push_back(other.status_);
			history_.push_back(status_);
			val_ = other.val_;
			other.val_ = 0;
			return *this;
		}

		virtual ~TestObject()
		{
			std::cout << "[object " << id_ << "] " << "virtual TestObject::~TestObject() (destructor)" << std::endl;
			status_ = "this object is destructed";
			history_.push_back(status_);
			--living_objects_count_;
		}

		friend std::ostream& operator<<(std::ostream& os, const TestObject& obj)
		{
			os << "TestObject{ id: " << obj.id_ << ", val: " << obj.val_ << " }";
			return os;
		}

		inline bool operator==(const TestObject& other) const noexcept { return val_ == other.val_; }
		inline bool operator!=(const TestObject& other) const noexcept { return val_ != other.val_; }
		inline bool operator< (const TestObject& other) const noexcept { return val_ < other.val_; }
		inline bool operator> (const TestObject& other) const noexcept { return val_ > other.val_; }
		inline bool operator<=(const TestObject& other) const noexcept { return val_ <= other.val_; }
		inline bool operator>=(const TestObject& other) const noexcept { return val_ >= other.val_; }

		inline const std::vector<std::string>& getHistory() const noexcept { return history_; }
		inline const std::string& getStatus() const noexcept { return status_; }
		inline int getId() const noexcept { return id_; }
		inline int getParentId() const noexcept { return parent_id_; }
		inline int getValue() const noexcept { return val_; }

	private:
		static int living_objects_count_;
		static std::size_t counter_;
		std::size_t id_ = 0;
		std::size_t parent_id_ = 0;
		int val_ = 0;
		std::string status_;
		std::vector<std::string> history_;
	};

	int TestObject::living_objects_count_ = 0;
	std::size_t TestObject::counter_ = 0;
}

#endif // TEST_OBJECT_HPP
