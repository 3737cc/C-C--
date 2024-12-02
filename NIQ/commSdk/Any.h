#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept> 

/**
 * @brief 一个类型安全的容器，可以存储任意类型的值。
 *
 * Any 类允许存储任意类型，并提供类型安全的方法来检索存储的值。
 *
 * 使用示例：
 * @code
 * Any a = 10; // 存储一个 int
 * std::cout << a.any_cast<int>() << std::endl; // 检索并打印 int
 *
 * Any b = std::string("Hello, World!");
 * std::cout << b.any_cast<std::string>() << std::endl; // 检索并打印 string
 * @endcode
 *
 * 作者: 江涛 205244751@qq.com
 * 版本: 1.0
 * 日期: 2024-07-12
 */
class Any {
public:
	Any() : type_index_(std::type_index(typeid(void))) {}

	/**
	 * @brief 构造一个存储给定值的 Any 对象。
	 *
	 * @tparam T 值的类型。
	 * @param value 要存储的值。
	 */
	template<typename T>
	Any(const T& value) : ptr_(new Derived<T>(value)), type_index_(std::type_index(typeid(T))) {}

	/**
	 * @brief 检查存储的值是否为指定类型。
	 *
	 * @tparam T 要检查的类型。
	 * @return true 如果存储的值为类型 T。
	 * @return false 否则。
	 */
	template<typename T>
	bool is() const {
		return type_index_ == std::type_index(typeid(T));
	}

	/**
	 * @brief 检索存储的值，如果它是指定的类型。
	 *
	 * @tparam T 要检索的值的类型。
	 * @return T& 对存储值的引用。
	 * @throws std::bad_cast 如果存储的值不是类型 T。
	 */
	template<typename T>
	T& any_cast() {
		if (!is<T>()) {
			throw std::bad_cast();
		}
		auto derived = dynamic_cast<Derived<T>*>(ptr_.get());
		return derived->value_;
	}

	/**
	 * @brief 检索存储的值，如果它是指定的类型（常量版本）。
	 *
	 * @tparam T 要检索的值的类型。
	 * @return const T& 对存储值的常量引用。
	 * @throws std::bad_cast 如果存储的值不是类型 T。
	 */
	template<typename T>
	const T& any_cast() const {
		if (!is<T>()) {
			throw std::bad_cast();
		}
		auto derived = dynamic_cast<const Derived<T>*>(ptr_.get());
		return derived->value_;
	}

	template<typename T>
	T& operator[](std::size_t index) {
		if (!is<T>()) {
			throw std::bad_cast();
		}
		auto derived = dynamic_cast<Derived<std::vector<T>>*>(ptr_.get());
		if (derived == nullptr) {
			throw std::bad_cast();
		}
		return derived->value_.at(index);
	}

private:
	struct Base {
		virtual ~Base() = default;
	};

	template<typename T>
	struct Derived : Base {
		Derived(const T& value) : value_(value) {}
		T value_;
	};

	std::shared_ptr<Base> ptr_;
	std::type_index type_index_;
};