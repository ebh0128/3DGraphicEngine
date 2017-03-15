#pragma once


//쉐이더를 감싸고있는 클래스
//쉐이더의 이름과 쉐이더를 이용하는 오브젝트 포인터들로 이루어짐

class PassShaderObject
{
protected:
	//패스에 이용될 쉐이더
	MyShader* m_pPassShader;

	std::vector<Object*> m_pPassObjectList;

//	void (PipeLine::*ApplyCallback)(Object*,MyShader*);

public:
	PassShaderObject(MyShader *passShader);

	void AddPassObject(Object* obj);

	//이 쉐이더의 이름과 똑같으면 true반환
	bool CheckName(char* newName);
	bool CheckName(std::string newName);
	void GetShaderName(char* retName);
	std::string GetNameString();
	MyShader* ApplyPassShader();
	int GetPassObjectCount();
	Object* GetDrawObject(int index);
//	void SetApplyCallback(void(PipeLine::*_CallBack)(Object*, MyShader*));
};