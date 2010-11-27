/**

	Singleton.h

*/

#pragma once

#include <assert.h>


#define INSTANCIATE_SINGLETON(Class) {assert(!Class::isInitialised()); new Class(); }
#define DESTROY_SINGLETON(Class) {assert(Class::isInitialised()); delete Class::getInstancePtr();}


namespace Flewnit
{
	template <typename T> class Singleton
	{
	public:

		//just a coding conveniance wrapper for the constructor ;)
//		static void startUp()
//		{
//			new T();
//		}

		Singleton( void )
		{
			assert( !mSingletonInstancePtr );
			mSingletonInstancePtr = static_cast<T*>( this );
		}

		virtual ~Singleton( void )
		{
			assert( mSingletonInstancePtr );
			mSingletonInstancePtr = 0;
		}
		static bool isInitialised( void )
		{
			return ( mSingletonInstancePtr != 0 );
		}
		static T& getInstance( void )
		{
			assert( mSingletonInstancePtr );
			return ( *mSingletonInstancePtr );
		}

		static T* getInstancePtr( void )
		{
			assert( mSingletonInstancePtr );
			return ( mSingletonInstancePtr );
		}


//		static void shutdown( void )
//		{
//			assert( mSingletonInstancePtr );
//			delete mSingletonInstancePtr;
//			mSingletonInstancePtr = 0;
//		}


	protected:
	private:

		Singleton( Singleton const& other );
		static T* mSingletonInstancePtr;

	}; // Singleton

	// static-initialations
	template <typename T> T* Singleton<T>::mSingletonInstancePtr = 0;
}

//-----------------------------------------------------------------------------
// End:				Singleton.h
//-----------------------------------------------------------------------------
