#pragma once


//���̴��� ���ΰ��ִ� Ŭ����
//���̴��� �̸��� ���̴��� �̿��ϴ� ������Ʈ �����͵�� �̷����

class PassShaderObject
{
protected:
	//�н��� �̿�� ���̴�
	MyShader* m_pPassShader;

	std::vector<Object*> m_pPassObjectList;


public:
	PassShaderObject(MyShader *passShader);

	void AddPassObject(Object* obj);

	//�� ���̴��� �̸��� �Ȱ����� true��ȯ
	bool CheckName(char* newName);
	bool CheckName(std::string newName);
	void GetShaderName(char* retName);
	std::string GetNameString();
	MyShader* ApplyPassShader();
};