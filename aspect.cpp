#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "object.hpp"
#include "objectptr.hpp"
#include "reflect.hpp"
#include "type.hpp"
#include "composite_type.hpp"
#include "struct_type.hpp"
#include "array_type.hpp"
#include "json_archive.hpp"
#include "universe.hpp"

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
	ObjectPtr<Foo> foo;
	Bar() : bar(456), foo(nullptr) {}
	~Bar() {
		std::cout << "~Bar " << this << '\n';
	}
	
	Signal<int32> when_something_happens;
};

BEGIN_TYPE_INFO(Bar)
	description("Bar is a class.");
	property(&Bar::bar, "bar", "Another number");
	property(&Bar::list, "list", "A list of numbers");
	property(&Bar::foo, "foo", "A reference to a foo");
	signal(&Bar::when_something_happens, "when_something_happens", "La la la");
END_TYPE_INFO()


int main (int argc, char const *argv[])
{
	TestUniverse universe;
	
	auto t = new CompositeType("FooBar");
	t->add_aspect(get_type<Foo>());
	t->add_aspect(get_type<Bar>());
	t->freeze();
	
	ObjectPtr<> p = universe.create_object(t, "Composite FooBar");
	ObjectPtr<Bar> b = universe.create<Bar>("Bar");
	std::cout << "p: " << p.get() << '\n';
	std::cout << "b: " << b.get() << '\n';
	
	ObjectPtr<Foo> foo = aspect_cast<Foo>(p);
	ObjectPtr<Bar> bar = aspect_cast<Bar>(p);
	bar->foo = foo;
	assert(bar->foo != nullptr);
	
	bar->when_something_happens.connect(foo, &Foo::a_signal_receiver);
	bar->when_something_happens(bar->bar);
	
	if (foo != nullptr) {
		std::cout << "Foo (" << foo << "): " << foo->foo << '\n';
		ObjectPtr<> comp = aspect_cast(foo, t);
		if (comp != nullptr) {
			std::cout << "Composite: " << comp << '\n';
		}
	}
	if (bar != nullptr) {
		std::cout << "Bar (" << bar << "): " << bar->bar << '\n';
		bar->list.push_back(1);
		bar->list.push_back(2);
		bar->list.push_back(3);
		ObjectPtr<> comp = aspect_cast(foo, t);
		if (comp != nullptr) {
			std::cout << "Composite: " << comp << '\n';
		}
	}
	
	JSONArchive json;
	json.serialize(p, universe);
	assert(bar->foo != nullptr);
	json.write(std::cout);
	
	return 0;
}
