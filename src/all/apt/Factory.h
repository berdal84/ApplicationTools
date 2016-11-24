#pragma once
#ifndef apt_Factory_h
#define apt_Factory_h

#include <apt/def.h>
#include <apt/StringHash.h>

#include <vector>

namespace apt {

////////////////////////////////////////////////////////////////////////////////
/// \class Factory
/// Templated factory base class. Provides static Create() and Destroy() 
/// functions to a deriving class via which subclasses may be instantiated.
/// Overhead per subclass is 1 ptr.
///	Usage example:
/// \code
/// // in the .h
///    class Entity: public apt::Factory<Entity>
///    {
///       virtual void update() = 0;
///    };
///
///    class Player: public Entity
///    {
///       virtual void update();
///    };
///
///    class Enemy: public Entity
///    {
///       static Entity* Create();          // user-defined create function
///       static void    Destroy(Entity*&); // user-defined destroy function
///
///       virtual void update();
///    };
///
/// // in the .cpp
///    APT_FACTORY_DEFINE(Entity);
///    APT_FACTORY_REGISTER_DEFAULT(Entity, Player);
///    APT_FACTORY_REGISTER(Entity, Enemy, Enemy::Create, Enemy::Destroy);
/// \endcode
///
/// Using APT_FACTORY_REGISTER_DEFAULT requires that the subclass have a default
/// ctor which takes no arguments (Create/Destroy simply call new/delete). For
/// user-defined Create/Destroy use the more generic APT_FACTORY_REGISTER.
///
/// Entity can now be used as a factory to manage instances of Player or Enemy:
/// \code
///    Entity* player  = Entity::Create("Player");
///    Entity* enemy   = Entity::Create("Enemy");
///    Entity::Destroy(player);
///    Entity::Destroy(enemy);
/// \endcode
///
/// It is also possible to iterate over the registered subclasses (useful for
/// generating drop down boxes for a UI):
/// \code
///    for (int i = 0; i < Entity::GetClassRefCount(); ++i) {
///       const Entity::ClassRef* cref = Entity::GetClassRef(i);
///       cref->getName(); // returns a const char* matching the class name
///       Entity* inst = Entity::Create(cref); // use the cref directly with Create().
///    }
/// \endcode
///
/// \todo Use a hash map instead of std::vector.
////////////////////////////////////////////////////////////////////////////////
template <typename tType>
class Factory
{
public:

	struct ClassRef
	{
		friend class Factory<tType>; // allow Factory access to create/destroy

		typedef tType* (CreateFunc)();
		typedef void   (DestroyFunc)(tType*&);
		
		ClassRef(const char* _name, CreateFunc* _create, DestroyFunc* _destroy)
			: m_name(_name)
			, m_nameHash(_name)
			, create(_create)
			, destroy(_destroy)
		{
			APT_ASSERT(FindClassRef(m_nameHash) == nullptr); // multiple registrations, or name was not unique
			s_registry.push_back(this);
		}

		const char* getName() const     { return m_name; }
		StringHash  getNameHash() const { return m_nameHash; }

	private:
		const char*  m_name;
		StringHash   m_nameHash;

	 // can't call these directly, use the static Create/Destroy functions on Factory
		CreateFunc*  create;
		DestroyFunc* destroy;
	};

	template <typename tSubType>
	struct ClassRefDefault: public ClassRef
	{
		static tType* Create()                                               { return new tSubType; }
		static void Destroy(tType*& _inst_)                                  { delete _inst_; _inst_ = nullptr; }
		ClassRefDefault(const char* _name): ClassRef(_name, Create, Destroy) {}
	};



	/// \return ClassRef corresponding to _nameHash, or 0 if not found.
	static const ClassRef* FindClassRef(StringHash _nameHash)
	{
		for (auto it = s_registry.begin(); it != s_registry.end(); ++it) {
			const ClassRef* cref = *it;
			if (cref->m_nameHash == _nameHash) {
				return cref;
			}
		}
		return 0;
	}

	/// \return Number of classes registered with the factory.
	static int GetClassRefCount()
	{
		return (int)s_registry.size();
	}

	/// \return _ith ClassRef registered with the factory.
	static const ClassRef* GetClassRef(int _i)
	{
		APT_ASSERT(_i < GetClassRefCount());
		return s_registry[_i];
	}

	/// \return Ptr to a new instance of the class specified by _name, or
	///   nullptr if an error occurred.
	static tType* Create(StringHash _name)
	{
		return Create(FindClassRef(_name));
	}

	/// \return Ptr to a new instance of the class specified by _cref, or
	///   nullptr if an error occurred.
	static tType* Create(const ClassRef* _cref)
	{
		APT_ASSERT(_cref);
		if (_cref) {
			tType* ret = _cref->create();
			ret->m_cref = _cref;
			return ret;
		}
		return nullptr;
	}

	/// Destroy a class instance previously created via Create().
	static void Destroy(tType*& _inst_)
	{
		APT_ASSERT(_inst_->m_cref);
		if (_inst_->m_cref) {
			_inst_->m_cref->destroy(_inst_);
		}
		_inst_ = nullptr;
	}

	const ClassRef* getClassRef() const { return m_cref; }

private:
	static std::vector<ClassRef*> s_registry; // \todo hash map 
	const ClassRef* m_cref;
};
#define APT_FACTORY_DEFINE(_baseClass) \
	std::vector<Factory<_baseClass>::ClassRef*> apt::Factory<_baseClass>::s_registry
#define APT_FACTORY_REGISTER(_baseClass, _subClass, _createFunc, _destroyFunc) \
	static Factory<_baseClass>::ClassRef s_ ## _subClass(#_subClass, _createFunc, _destroyFunc);
#define APT_FACTORY_REGISTER_DEFAULT(_baseClass, _subClass) \
	static Factory<_baseClass>::ClassRefDefault<_subClass> s_ ## _subClass(#_subClass)

} // namespace apt

#endif // apt_Factory_h
