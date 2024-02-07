#pragma once

template<typename T> class List{
	public:
	int count;
	int max;
	List(int _max){
		max = _max;
		arr = new T[max];
		count = 0;
	}
	~List(){
		delete arr;
	}

	T* add(){
		if(count >= max)
			return nullptr;
		return arr + count++;
	}
	void remove(int ind){
		for(int i = ind; i < count - 1; i++){
			T* node = (arr + i);
			*node = *(arr + i + 1);
		}
		count--;
	}
	T* get(int ind){
		return arr + ind;
	}

	void foreach(void(*func)(int ind, T* t)){
		for(int i = 0; i < count; i++){
			func(i, get(i));
		}
	}

	void moveUp(int ind){
		if(ind + 1 < max){
			T node = *(arr + ind + 1);
			*(arr + ind + 1) = *(arr + ind);
			*(arr + ind) = node;
		}
	}

	void moveDown(int ind){
		if(ind - 1 > -1){
			T node = *(arr + ind - 1);
			*(arr + ind - 1) = *(arr + ind);
			*(arr + ind) = node;
		}
	}
	private:
	T* arr;
};