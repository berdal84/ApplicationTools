#pragma once
#ifndef apt_static_initializer_h
#define apt_static_initializer_h

namespace apt {

////////////////////////////////////////////////////////////////////////////////
/// \class static_initializer
/// Implementation of the Nifty/Schwarz counter idiom (see
///   https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Nifty_Counter). 
/// Usage example:
/// \code
/// // in the .h
///    class Monostate
///    {
///    protected: // must be either protected or public
///       void Init();
///       void Shutdown();
///    };
///    APT_DECLARE_STATIC_INIT(MonoState);
///
/// // in the .cpp
///    void Monostate::Init() {}
///    void Monostate::Shutdown() {}
///    APT_DEFINE_STATIC_INIT(MonoState);
/// \endcode
/// \note Init() should not construct any non-trivial static objects as the
///   order of initialization relative to static_initializer cannot be
///   guaranteed. This means that static objects initialized during Init()
///   may subsequently be default-initialized, overwriting the value set by
///   Init(). To get around this, use the storage class (memory.h), e.g.
/// \code
///   #inclide <apt/memory.h>
///   static storage<Foo, 1> g_foo;
///
///   void Monostate::Init()
///   {
///      new(g_foo) Foo();
///      // ..
/// \endcode
/// \ingroup APT_core
////////////////////////////////////////////////////////////////////////////////
template <typename tType>
class static_initializer: private tType
{
	static int s_initCounter;
public:
	static_initializer()  { if (++s_initCounter == 1) { Init(); } }
	~static_initializer() { if (--s_initCounter == 0) { Shutdown(); } }
};
#define APT_DECLARE_STATIC_INIT(t) static apt::static_initializer<t> t ## _static_initializer
#define APT_DEFINE_STATIC_INIT(t)  int apt::static_initializer<t>::s_initCounter = 0


} // namespace apt

#endif // apt_static_initializer_h
