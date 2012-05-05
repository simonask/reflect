#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "object.hpp"
#include "reflect.hpp"
#include "type.hpp"
#include "composite_type.hpp"
#include "struct_type.hpp"
#include "array_type.hpp"
#include "json_archive.hpp"

struct Foo : Object {
	REFLECT;
	
	int foo;
	void a_signal_receiver(int n) { std::cout << "Foo::a_signal_receiver: " << n << "\n"; an_empty_signal(); }
	Signal<int32> after_signal_received;
	Signal<> an_empty_signal;
	
	Foo() : foo(123) {}
};

BEGIN_TYPE_INFO(Foo)
	description("Foo is a class.");
	property(&Foo::foo, "foo", "A number.");
	slot(&Foo::a_signal_receiver, "Signal receiver", "It's a slot.");
	signal(&Foo::after_signal_received, "After signal received", "It's a signal.");
	signal(&Foo::an_empty_signal, "Empty signal", "blah blah");
END_TYPE_INFO()

struct Bar : Object {
	REFLECT;
	
	int bar;
	std::vector<int> list;
	Bar() : bar(456) {}
	~Bar() {
		std::cout << "~Bar" << '\n';
	}
	
	Signal<int32> when_something_happens;
};

BEGIN_TYPE_INFO(Bar)
	description("Bar is a class.");
	property(&Bar::bar, "bar", "Another number");
	property(&Bar::list, "list", "A list of numbers");
END_TYPE_INFO()


template <typename T>
typename std::enable_if<std::is_convertible<T*, Object*>::value, T*>::type
create() {
	auto type = static_cast<const StructTypeImpl<T>*>(get_type<T>());
	byte* memory = new byte[sizeof(T)];
	type->construct(memory);
	T* object = reinterpret_cast<T*>(memory);
	object->set_object_type__(type);
	object->set_object_offset__(0);
	return object;
}

Object* create(const DerivedType* type) {
	byte* memory = new byte[type->size()];
	type->construct(memory);
	Object* object = reinterpret_cast<Object*>(memory);
	object->set_object_type__(type);
	return object;
}

template <typename T>
void destroy_impl(typename std::enable_if<std::is_convertible<T*, Object*>::value, T*>::type ptr) {
	Object* topmost = ptr->find_topmost_object();
	topmost->object_type()->destruct(reinterpret_cast<byte*>(topmost));
	byte* memory = reinterpret_cast<byte*>(topmost);
	delete[] memory;
}

template <typename T>
void destroy_impl(typename std::enable_if<!std::is_convertible<T*, Object*>::value, T*>::type ptr) {
	delete ptr;
}

template <typename T>
void destroy(T* ptr) {
	destroy_impl<T>(ptr);
}

template <typename T>
struct ObjectDestroyer {
	void operator()(T* ptr) {
		destroy(ptr);
	}
};

template <typename T>
using UPtr = std::unique_ptr<T, ObjectDestroyer<T>>;

template <typename T>
UPtr<T> create_unique() {
	return UPtr<T>(create<T>());
}

UPtr<Object> create_unique(const DerivedType* type) {
	return UPtr<Object>(create(type));
}

int main (int argc, char const *argv[])
{
	auto t = new CompositeType("FooBar");
	t->add_aspect(get_type<Foo>());
	t->add_aspect(get_type<Bar>());
	t->freeze();
	
	auto p = create_unique(t);
	auto b = create_unique<Bar>();
	std::cout << "p: " << p.get() << '\n';
	std::cout << "b: " << b.get() << '\n';
	
	Foo* foo = aspect_cast<Foo>(p.get());
	Bar* bar = aspect_cast<Bar>(p.get());
	
	bar->when_something_happens.connect(foo, &Foo::a_signal_receiver);
	bar->when_something_happens(bar->bar);
	
	if (foo != nullptr) {
		std::cout << "Foo (" << foo << "): " << foo->foo << '\n';
		Object* comp = aspect_cast(foo, t);
		if (comp != nullptr) {
			std::cout << "Composite: " << comp << '\n';
		}
	}
	if (bar != nullptr) {
		std::cout << "Bar (" << bar << "): " << bar->bar << '\n';
		bar->list.push_back(1);
		bar->list.push_back(2);
		bar->list.push_back(3);
		Object* comp = aspect_cast(foo, t);
		if (comp != nullptr) {
			std::cout << "Composite: " << comp << '\n';
		}
	}
	
	JSONArchive json;
	t->serialize(reinterpret_cast<const byte*>(p.get()), json.root());
	json.write(std::cout);
	
	return 0;
}
