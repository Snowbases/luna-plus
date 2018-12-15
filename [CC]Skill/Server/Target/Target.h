#pragma once

class CObject;

class cTarget
{
protected:
	typedef DWORD ObjectIndex;
	typedef std::set< ObjectIndex > ObjectIndexContainer;
	ObjectIndexContainer mObjectIndexContainer;
	ObjectIndexContainer::const_iterator mIterator;

public:
	cTarget();
	virtual ~cTarget();
	void SetPositionHead() { mIterator = mObjectIndexContainer.begin(); }
	CObject* GetData();
	void AddData(ObjectIndex objectIndex) { mObjectIndexContainer.insert(objectIndex); }
	void RemoveAll() { mObjectIndexContainer.clear(); }
	size_t GetSize() const { return mObjectIndexContainer.size(); }
};
