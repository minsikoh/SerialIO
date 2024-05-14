#pragma once

template<class T>
class CSingleton
{
public:
	CSingleton(void) {}
	virtual ~CSingleton(void) {}

	static T& getInstance()
	{
		if (0 == m_pInstance)
			m_pInstance = new T();

		return *m_pInstance;
	}

	void freeInstance()		// instance ªË¡¶
	{
		removeAll();

		delete m_pInstance;
		m_pInstance = nullptr;
		//SAFE_DELETE( m_pInstance );
	}

	virtual void removeAll()
	{

	}


public:
	static T* m_pInstance;
};

template<class T> T* CSingleton<T>::m_pInstance = 0;