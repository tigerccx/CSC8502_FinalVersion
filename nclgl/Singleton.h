#pragma once
template<class T>
class Singleton {
public:
	static T* GetInstance(){
		return &__instance;
	}
	static T __instance;
};

template<class T> T Singleton<T>::__instance;