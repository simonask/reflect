C++ reflection and serialization framework
==========================================

This is a simple library that allows for simple reflection and serialization of C++ types. It is designed for use in video game engines, or other rich environments in which runtime object creation and reflection is useful.

Features
--------

* No meta-object compiler / preprocessing build step.
* Composite types ("aspect oriented programming") with rich interface casts.
* Simple serialization (to JSON at the moment).
* Simple and efficient signal/slot implementation included.
* Type-safe.
* C++11 compliant and extensible -- for instance, serializers for custom types can be easily defined, without modification to those types.

Planned/pending features
------------------------

* Serialization of signal/slot connections.
* Serialization of object references.
* YAML serialization.
* XML serialization.
* Built-in support for serialization of more standard library types.
* Type-safe public serialization API.
* Central type registry for enumeration of all types.

Limitations
-----------

* Objects must derive from the `Object` type and include the `REFLECT` tag in their definition.
* Rich casting (`aspect_cast`) requires runtime type information.
* Members of objects that aren't described by a `property(member, name, description)` will not be serialized/deserialized.

Examples
========

In foo.hpp:


    #include "object.hpp"
    
    class Foo : public Object {
        REFLECT;
    public:
        Foo() {}
        void say_hi() { std::cout << "Hi!\n"; }
    private:
        int32                an_integer_property;
        std::vector<float32> a_list_of_floats;
    };

In foo.cpp:

    #include "foo.hpp"
    #include "reflect.hpp"
    
    BEGIN_TYPE_INFO(Foo)
        property(&Foo::an_integer_property, "Number",     "An integer property.");
        property(&Foo::a_list_of_floats,    "Float list", "A list of floats.");

        slot(&Foo::say_hi, "Say Hi!", "A slot that says hi.");
    END_TYPE_INFO()

In main.cpp:

    int f() {
        // Print the type name and all properties.
        auto foo_type = get_type<Foo>();
        std::cout << foo_type->name() << '\n'; // prints "Foo"
        for (auto attribute: foo_type->attribute()) {
            std::cout << attribute->name() << ": " << attribute->type()->name() << '\n';
        }

        // Create a composite type consisting of two Foos.
        CompositeType* composite = new CompositeType("FooFoo");
        composite->add_aspect(get_type<Foo>());
        composite->add_aspect(get_type<Foo>());
        Object* c = create(composite);
        Foo* f = aspect_cast<Foo>(c); // get a pointer to the first Foo in c.
        f->say_hi();
        
        // Serialize the composite as JSON, and write to stdout.
        JSONArchive archive;
        composite->serialize((byte*)c, archive.root());
        archive.write(std::cout);
    }