#pragma once

#include <algorithm>
//#include <cassert>
#include <stdexcept>

template <typename Type>
class SingleLinkedList {
	struct Node {
		Node() = default;
		Node(const Type& val, Node* next) : value(val), next_node(next) { }
		Type value;
		Node* next_node = nullptr;
	};

public:         // constructors
	SingleLinkedList();
	~SingleLinkedList();
	SingleLinkedList(std::initializer_list<Type> values);
	SingleLinkedList(const SingleLinkedList& other);

	// no idea how to place template nested class methods definitions outside of the class declaration
private:        // Basic Iterator
	// ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
	template <typename ValueType>
	class BasicIterator {
		// Класс списка объявляется дружественным, чтобы из методов списка
		// был доступ к приватной области итератора
		friend class SingleLinkedList;
		// Конвертирующий конструктор итератора из указателя на узел списка
		explicit BasicIterator(Node* node) {
			node_ = node;
		}

	public:
		// Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора
		// Категория итератора — forward iterator
		// (итератор, который поддерживает операции инкремента и многократное разыменование)
		using iterator_category = std::forward_iterator_tag;
		// Тип элементов, по которым перемещается итератор
		using value_type = Type;
		// Тип, используемый для хранения смещения между итераторами
		using difference_type = std::ptrdiff_t;
		// Тип указателя на итерируемое значение
		using pointer = ValueType*;
		// Тип ссылки на итерируемое значение
		using reference = ValueType&;

		BasicIterator() = default;

		// Конвертирующий конструктор/конструктор копирования
		// При ValueType, совпадающем с Type, играет роль копирующего конструктора
		// При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
		BasicIterator(const BasicIterator<Type>& other) noexcept {
			node_ = other.node_;
		}

		BasicIterator& operator=(const BasicIterator& rhs) = default;

		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			if (node_ == nullptr && rhs.node_ == nullptr)
				return true;
			if (node_ == nullptr || rhs.node_ == nullptr)
				return false;
			if (node_ == rhs.node_)
				return true;
			return false;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			return !(*this == rhs);
		}

		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			return node_ == rhs.node_;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			return *this == rhs ? false : true;
		}

		// assert works in debug mode only. Throw - stays in release. But no "noexcept" annotation
		BasicIterator& operator++() /*noexcept*/ {
			// assert(this != nullptr);
			if (this == nullptr)
				throw std::out_of_range("++nullptr");
			node_ = node_->next_node;
			return *this;
		}

		BasicIterator operator++(int) /*noexcept*/ {
			if (this == nullptr)
				throw std::out_of_range("nullptr++");
			BasicIterator tmp = *this;
			++(*this);
			return tmp;
		}

		[[nodiscard]] reference operator*() const /*noexcept*/ {
			if (this == nullptr)
				throw std::out_of_range("*nullptr");
			return node_->value;
		}

		[[nodiscard]] pointer operator->() const /*noexcept*/ {
			if (this == nullptr)
				throw std::out_of_range("nullptr->");
			return &(node_->value);
		}

	private:
		Node* node_ = nullptr;
	};		// End of nested class BasicIterator<ValueType>

public:     // public iterators
	using value_type = Type;
	using reference = value_type&;
	using const_reference = const value_type&;

	// Итератор, допускающий изменение элементов списка
	using Iterator = BasicIterator<Type>;
	// Константный итератор, предоставляющий доступ для чтения к элементам списка
	using ConstIterator = BasicIterator<const Type>;

	[[nodiscard]] Iterator begin() noexcept {
		Iterator ret(head_.next_node);
		return ret;
	}

	[[nodiscard]] ConstIterator begin() const noexcept {
		const Iterator ret(head_.next_node);
		return ret;
	}

	[[nodiscard]] ConstIterator cbegin() const noexcept {
		return begin();
	}

	[[nodiscard]] Iterator end() noexcept {
		Iterator ret(nullptr);
		return ret;
	}

	[[nodiscard]] ConstIterator end() const noexcept {
		const Iterator ret(nullptr);
		return ret;
	}

	[[nodiscard]] ConstIterator cend() const noexcept {
		return end();
	}

	[[nodiscard]] Iterator before_begin() noexcept {
		Iterator ret(&head_);
		return ret;
	}

	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		const Node* tmp = &head_;
		ConstIterator ret(const_cast<Node*>(tmp));
		return ret;
	}

	[[nodiscard]] ConstIterator before_begin() const noexcept {
		const ConstIterator ret(&head_);
		return ret;
	}

public:         // methods
	[[nodiscard]] size_t GetSize() const noexcept;
	[[nodiscard]] bool IsEmpty() const noexcept;
	void PushFront(const Type& value);
	void PushBack(const Type& value);
	void Clear() noexcept;
	void swap(SingleLinkedList& other) noexcept;
	void PopFront() noexcept;

	Iterator InsertAfter(ConstIterator pos, const Type& value) {		// can't segregate declaration and definition
		Node* tmp = new Node(value, pos.node_->next_node);
		ConstIterator pos_tmp = pos;
		pos.node_->next_node = tmp;
		++pos;
		Iterator ret(pos.node_);
		++size_;
		return ret;
	}

	Iterator EraseAfter(ConstIterator pos) noexcept {
		Node* to_delete = pos.node_->next_node;
		pos.node_->next_node = to_delete->next_node;
		delete to_delete;
		++pos;
		Iterator ret(pos.node_);
		--size_;
		return ret;
	}

public:			// operators
	SingleLinkedList& operator=(const SingleLinkedList& rhs);

private:        // fields
	Node head_;
	size_t size_ = 0;
};

template<typename Type>
SingleLinkedList<Type>::SingleLinkedList()
{
	size_ = 0;
}

template<typename Type>
SingleLinkedList<Type>::~SingleLinkedList() {
	Clear();
}

template<typename Type>
SingleLinkedList<Type>::SingleLinkedList(std::initializer_list<Type> values) {
	for (Type i : values) {
		PushBack(i);
	}
}

template<typename Type>
SingleLinkedList<Type>::SingleLinkedList(const SingleLinkedList& other) {
	size_t sz_t = other.GetSize();
	SingleLinkedList::ConstIterator curr = other.begin();

	for (size_t i = 0; i < sz_t; ++i) {
		PushBack(curr.node_->value);
		++curr;
	}
}

template<typename Type>
size_t SingleLinkedList<Type>::GetSize() const noexcept
{
	return size_;
}

template<typename Type>
bool SingleLinkedList<Type>::IsEmpty() const noexcept {
	return size_ == 0;
}

template<typename Type>
void SingleLinkedList<Type>::PushFront(const Type& value) {
	head_.next_node = new Node(value, head_.next_node);
	++size_;
}

template<typename Type>
void SingleLinkedList<Type>::PushBack(const Type& value) {
	if (size_ == 0) {
		Node* n = new Node(value, nullptr);
		head_.next_node = n;
		++size_;
		return;
	}

	Node* t = head_.next_node;
	for (size_t i = 1; i < size_; ++i) {
		t = t->next_node;
	}

	Node* n = new Node(value, nullptr);
	t->next_node = n;
	++size_;
}

template<typename Type>
void SingleLinkedList<Type>::Clear() noexcept {
	if (size_ == 0 || head_.next_node == nullptr) return;
	do {
		Node* tmp = head_.next_node->next_node;
		delete head_.next_node;
		head_.next_node = tmp;
	} while (head_.next_node != nullptr);
	size_ = 0;
}

template<typename Type>
void SingleLinkedList<Type>::swap(SingleLinkedList& other) noexcept {
	std::swap(size_, other.size_);
	std::swap(head_.next_node, other.head_.next_node);
}

template<typename Type>
void SingleLinkedList<Type>::PopFront() noexcept {
	Node* tmp = head_.next_node->next_node;
	delete head_.next_node;
	head_.next_node = tmp;
	--size_;
}

template<typename Type>
SingleLinkedList<Type>& SingleLinkedList<Type>::operator=(const SingleLinkedList<Type>& rhs) {
	if (&rhs == this) 
		return *this;
	SingleLinkedList tmp(rhs);
	this->swap(tmp);
	return *this;
}


template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	if (lhs.GetSize() != rhs.GetSize())
		return false;
	return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return lhs == rhs ? false : true;
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
		[](Type left, Type right) { return left < right; });
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs > rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
		[](Type left, Type right) { return left > right; });
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs < rhs);
}
