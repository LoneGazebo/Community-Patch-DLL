
#pragma once
#ifndef ICVDLLSCRIPTSYSTEM_H
#define ICVDLLSCRIPTSYSTEM_H

struct lua_State;

class ICvEngineScriptSystemArgs1
{
public:
	enum ArgType
	{
		ARGTYPE_NONE,
		ARGTYPE_NULL,
		ARGTYPE_BOOL,
		ARGTYPE_STRING,
		ARGTYPE_INT,
		ARGTYPE_FLOAT,
	};
	virtual ~ICvEngineScriptSystemArgs1(){};

	virtual unsigned int Push(bool bValue) = 0;
	virtual unsigned int Push(const char* szValue, size_t lenValue) = 0;
	virtual unsigned int Push(int iValue) = 0;
	virtual unsigned int Push(float fValue) = 0;
	virtual unsigned int PushNULL() = 0;

	virtual unsigned int Count() = 0;
	
	virtual ArgType GetType(unsigned int argIndex) = 0;

	virtual bool GetBool(unsigned int argIndex, bool& bValue) = 0;
	virtual bool GetFloat(unsigned int argIndex, float& fValue) = 0;
	virtual bool GetInt(unsigned int argIndex, int& iValue) = 0;
	virtual bool GetString(unsigned int argIndex, char*& szValue) = 0;
};

class ICvEngineScriptSystem1
{
public:

	virtual lua_State* CreateLuaThread(_In_z_ const char* szName) = 0;
	virtual void FreeLuaThread(_In_opt_ lua_State*) = 0;

	// File IO
	virtual bool LoadFile(_In_ lua_State* L, _In_z_ const char* szFilename) = 0;

	//Arguments
	virtual __checkReturn ICvEngineScriptSystemArgs1* CreateArgs() = 0;
	virtual __checkReturn ICvEngineScriptSystemArgs1* CreateArgs(uint uiReserve) = 0;
	virtual void DestroyArgs(_In_opt_ ICvEngineScriptSystemArgs1* pkArgs) = 0;

	// Method calling
	virtual bool CallCFunction(_In_ lua_State* L, lua_CFunction func, void* args) = 0;
	virtual bool CallFunction(_In_ lua_State* L, unsigned int nArgs, unsigned int nRet) = 0;

	virtual bool CallHook(_In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value) = 0;

	virtual bool CallAccumulator(_In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, int& value) = 0;
	virtual bool CallAccumulator(_In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, float& value) = 0;

	virtual bool CallTestAll(_In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value) = 0;
	virtual bool CallTestAny(_In_z_ const char* szName, _In_opt_ ICvEngineScriptSystemArgs1* args, bool& value) = 0;
};


#endif //ICVDLLSCRIPTSYSTEM_H