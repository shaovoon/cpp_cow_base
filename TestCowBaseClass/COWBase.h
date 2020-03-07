#pragma once

template<typename T>
class COWBase
{
protected:
	// call this function in derived constructor
	void construct()
	{
		m_ptr = std::make_shared<T>();
	}
	// call this function in derived's setter before other code
	void clone_if_needed()
	{
		if (m_ptr.use_count() > 1)
		{
			std::shared_ptr<T> old = m_ptr;
			construct();
			*m_ptr = *old; // copy the old contents to new ptr.
		}
	}
	// function to get the internal raw ptr
	const T* ptr() const
	{
		return m_ptr.get();
	}
	// function to get the internal raw ptr
	T* ptr()
	{
		return m_ptr.get();
	}
	// returns count of the shared_ptr instance
	long use_count() const
	{
		return m_ptr.use_count();
	}
private:
	std::shared_ptr<T> m_ptr;
};
