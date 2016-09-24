#ifndef _TYPES_H_
#define _TYPES_H_

class Types
{
public:
	typedef enum
	{
		PT_ENGINE = 0,
		PT_RADAR,
		PT_FUEL_TANK,
		PT_AMMO_BOX,
		PT_DISSIPATOR,
		PT_REACTOR,
		PT_ACCUMULATOR,
		PT_SHIELD
	} EquipementType;

	typedef enum
	{
		ET_CHIMICAL = 0
	} EngineType;

	typedef enum
	{
		RT_CHIMICAL = 0,
		RT_ANTIMATTER,
		RT_FUSION,
		RT_NUCLEAR
	} ReactorType;

	static EquipementType getEquipementType(const std::string& _type)
	{
		if(_type == "engine")
			return PT_ENGINE;
		else if(_type == "radar")
			return PT_RADAR;
		else if(_type == "fuelTank")
			return PT_FUEL_TANK;
		else if(_type == "ammoBox")
			return PT_AMMO_BOX;
		else if(_type == "dissipator")
			return PT_DISSIPATOR;
		else if(_type == "reactor")
			return PT_REACTOR;
		else if(_type == "accumulator")
			return PT_ACCUMULATOR;
		else if(_type == "shield")
			return PT_SHIELD;

		//TODO wtf
		return PT_ENGINE;
	}

	static EngineType getEngineType(const std::string& _type)
	{
		if(_type == "chimical")
			return ET_CHIMICAL;

		//TODO wtf
		return ET_CHIMICAL;
	}

	static ReactorType getReactorType(const std::string& _type)
	{
		if(_type == "chimical")
			return RT_CHIMICAL;
		else if(_type == "antimatter")
			return RT_ANTIMATTER;
		else if(_type == "fusion")
			return RT_FUSION;
		else if(_type == "nuclear")
			return RT_NUCLEAR;

		//TODO wtf
		return RT_CHIMICAL;
	}
};

#endif //_TYPES_H_