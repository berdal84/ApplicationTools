#pragma once
#ifndef apt_static_initializer_h
#define apt_static_initializer_h

namespace apt {

////////////////////////////////////////////////////////////////////////////////
/// \class static_initializer
/// Implementation of the Nifty/Schwarz counter idiom (see
///   https://john-chapman.github.io/2016/09/01/static-initialization.html). 
/// Usage example:
/// \code
/// // in the .h
///    class Foo
///    {
///       void Init();
///       void Shutdown();
///    };
///    APT_DECLARE_STATIC_INIT(Foo, Foo::Init, Foo::Shutdown);
///
/// // in the .cpp
///    void Foo::Init()     { /* do init here */ }
///    void Foo::Shutdown() { /* do shutdown here */ }
///    APT_DEFINE_STATIC_INIT(Foo);
/// \endcode
/// \note Init() should not construct any non-trivial static objects as the
///   order of initialization relative to static_initializer cannot be
///   guaranteed. This means that static objects initialized during Init()
///   may subsequently be default-initialized, overwriting the value set by
///   Init(). To get around this, use heap allocation or the storage class 
///   (memory.h):
/// \code
///   #inclide <apt/memory.h>
///   static storage<Bar, 1> s_bar;
///
///   void Foo::Init()
///   {
///      new(s_bar) Bar();
///      // ..
/// \endcode
////////////////////////////////////////////////////////////////////////////////
template <typename tType>
class static_initializer
{
public:
	typedef void (Callback)(void);

	static_initializer(Callback* _onInit, Callback* _onShutdown)
	{ 
		if_unlikely (++s_initCounter == 1) {
			s_onShutdown = _onShutdown;
			_onInit();
		} 
	}
	~static_initializer()
	{
		if_unlikely (--s_initCounter == 0) { 
			if (s_onShutdown) {
				s_onShutdown();
			}
		} 
	}
private:
	static int s_initCounter;
	static Callback* s_onShutdown;
};
#define APT_DECLARE_STATIC_INIT(_type, _onInit, _onShutdown) static apt::static_initializer<_type> _type ## _static_initializer(_onInit, _onShutdown)
#define APT_DEFINE_STATIC_INIT(_type)  int apt::static_initializer<_type>::s_initCounter; apt::static_initializer<_type>::Callback* apt::static_initializer<_type>::s_onShutdown


} // namespace apt

#endif // apt_static_initializer_h
