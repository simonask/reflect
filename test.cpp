#include <iostream>
#include <string>

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
#include "maybe.hpp"
#include "maybe_type.hpp"
#include "type_registry.hpp"

struct Foo : Object {
	REFLECT;
	
	Maybe<int> foo;
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
	Array<int> list;
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
	TypeRegistry::add<Object>();
	TypeRegistry::add<Foo>();
	TypeRegistry::add<Bar>();
	
	TestUniverse universe;
	
	auto t = new CompositeType("FooBar");
	t->add_aspect(get_type<Foo>());
	t->add_aspect(get_type<Bar>());
	t->freeze();
	
	ObjectPtr<> p = universe.create_object(t, "Composite FooBar");
	ObjectPtr<Bar> b = universe.create<Bar>("Bar");
	
	ObjectPtr<Bar> bar = aspect_cast<Bar>(p);
	ObjectPtr<Foo> foo = aspect_cast<Foo>(p);
	bar->when_something_happens.connect(foo, &Foo::a_signal_receiver);
	bar->when_something_happens(bar->bar);
	
	JSONArchive json;
	json.serialize(p, universe);
	json.write(std::cout);
	
	TestUniverse universe2;
	ObjectPtr<> root = json.deserialize(universe2);
	ObjectPtr<Bar> bar2 = aspect_cast<Bar>(root);
	bar2->when_something_happens(bar2->bar);
	return 0;
}
