#pragma once
using namespace std;
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>
/*Nen doc ve ECS tren trang web cua Austin Morlan (toi gui len group chat) truoc tai ong day giai thich rat ki cang (nhung van cuc ki kho hieu :)))*/

/*ECS chu yeu quan ly dua tren array, thay vi cac class duoc ke thua nhu lap trinh binh thuong. No duoc dung de tranh truong hop
co mot object nao do can duoc inherit ca hai hay nhieu object khac*/

/*ve ly thuyet la dung array, nhung trong day tac gia dung vector de quan ly do cac uu diem cua no (cac ong deu biet roi)*/

/*
Component nhu la cac thuoc tinh cua Entity (doi tuong)
Entity hieu don gian la mot doi tuong (player, wall, enemy)
Manager dung de quan ly cac Component
*/

/*moi mot object gio day se la entity, moi mot entity se co cac component khac nhau va duoc quan ly voi mot system (class Manager) nao do*/

class Component;
class Entity;
class Manager;

using ComponentID = size_t;
using Group = size_t;


/*Cho component moi mot ID moi
De y rang lastID la static, tuc la no tang len cho du minh goi lai function nay
 */
inline ComponentID getNewComponentTypeID() {
	static ComponentID lastID = 0u;
	return lastID++;
}


/* get ID cho component*/
template <typename T> inline ComponentID getComponentTypeID() noexcept {
	static ComponentID typeID = getNewComponentTypeID();
	return typeID;
}

/* Cai nay co nghia la co toi da 32 components va 32 group (cua cac entity):
Dung group voi y tuong la nhom cac entity cung loai (VD: group cua players entities, group cua enemies entities,...

*/
constexpr size_t maxComponents = 32;
constexpr size_t maxGroups = 32;

/* may cai using nay toi cung khong hieu lam :))))), co ma cac ong nhin code ben duoi thi chac la co the hieu no so so. Toi nghi la 
no giong nhu minh khai bao mang bool[] hay array cua Components binh thuong, nhung co gioi han (nhu ComponentArray bi gioi han maxComponents thanh phan,
hoac duoc nen lai theo kieu bitset (co the de tiet kiem bo nho hay gi do @@)*/
using ComponentBitSet = bitset<maxComponents>;
using GroupBitset = bitset<maxGroups>;


using ComponentArray = array<Component*, maxComponents>;
class Component {
public:
	Entity* entity;
	virtual void init() {

	}
	virtual void update(){}
	virtual  void draw(){}
	virtual ~Component(){}

};

class Entity {
private:
	/*khai bao thong thuong*/
	Manager& manager;
	bool active = true;
	vector<unique_ptr<Component>> components;
	ComponentArray componentArray;
	ComponentBitSet componentBitSet;
	GroupBitset groupBitset;

public:

	/*video toi hoc thi tac gia chua lam gi voi cai nay, tam thoi cu de nhu the nay*/
	Entity(Manager& mManager) : manager(mManager) {

	}
	/*cac cai function nay nhin la biet roi*/
	void update() {
		for (auto& c : components)c->update();
		
	}
	void draw(){
		for (auto& c : components)c->draw();
	}
	bool isActive() const { return active; }
	void destroy() { active = false; }

	bool hasGroup(Group mGroup) {
		return groupBitset[mGroup];
	}

	void addGroup(Group mGroup);
	void delGroup(Group mGroup)
	{
		groupBitset[mGroup] = false;
	}


	template <typename T> bool hasComponent() const {
		return componentBitSet[getComponentTypeID<T>()];
	}
	/*chi don gian la add component thoi, cau truc ECS no co may cai array gi do nen moi phuc tap nhu ben duoi*/
	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs) {
		T* c(new T(forward<TArgs>(mArgs)...));
		c->entity = this;
		/*Giai thich tu wiki: std::unique_ptr is a smart pointer that owns and manages another object through a pointer and disposes of that object when the unique_ptr goes out of scope.*/
		unique_ptr<Component> uPtr{ c };

		//vector components chua cac component object (khong biet co dung khong, cac ong thay sai thi sua lai giup nhe)
		components.emplace_back(move(uPtr));

		//Array nay chua cac ID cua component
		componentArray[getComponentTypeID<T>()] = c;

		//Array nay chua trang thai cua component (duoc phan biet dua vao ID) do
		componentBitSet[getComponentTypeID<T>()] = true;

		c->init();
		return *c;
	}

	template<typename T> T& getComponent() const {
		auto ptr(componentArray[getComponentTypeID<T>()]);
		/*dung static cast de tranh tai nan do cast kieu khac(const cast hoac reinterpret cast) do
		binh thuong minh cast kieu (double)c thi tuc la c trong mot so truong hop nhat dinh co the bi cast sang kieu const hoac reinterpret
		 */
		return *static_cast<T*>(ptr);
	}


};

class Manager {
private:
	/*hieu nom na la array chua cac element la mot vector, vector do lai chua cac entity trong chinh no*/
	vector<unique_ptr<Entity>> entities;
	array<vector<Entity*>, maxGroups> groupedEntities;

public:
	void update() {
		for (auto& e : entities) e->update();
	}
	void draw() {
		for (auto& e : entities) e->draw();
	}
	void refresh() {
		//0u tuc la so 0 nay la usigned int, khong phai int binh thuong
		for (auto i(0u); i < maxGroups; i++) {
			//phan nay khong hieu lam, toi se doc tai lieu va update sau :))) (dau dau vl)
			auto& v(groupedEntities[i]);
			v.erase(
				remove_if(begin(v),end(v),
					[i](Entity* mEntity)
					{
						return !mEntity->isActive() || !mEntity->hasGroup(i);
					}

			), end(v));
		}
		
		entities.erase(remove_if(begin(entities), end(entities), [](const unique_ptr<Entity>& mEntity)
			{
				return !mEntity->isActive();
			}),
			end(entities)
				);
	}
	/*cai nay lien quan den group cua cac entity nhu da noi o tren, cu phap cua no tuong doi de hieu*/
	void AddToGroup(Entity* mEntity, Group mGroup) {
		groupedEntities[mGroup].emplace_back(mEntity);
	}

	vector<Entity*>& getGroup(Group mGroup) {
		return groupedEntities[mGroup];

	}
	
	Entity& addEntity() {
		Entity* e = new Entity(*this);
		unique_ptr<Entity> uPtr{ e };

		//vector chua cac entity
		entities.emplace_back(move(uPtr));
		return *e;
	}
};