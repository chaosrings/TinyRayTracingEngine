#include "parser.h"
regex Parser::fileRegex=regex(".+\\.\\w+");   //��̬��Ա��ʼ��
/*  .ƥ���ַ���\\.ƥ��. \\wƥ����ĸ�����»���*/

Token Parser::GetNextToken()
{
	while (toParse[index] == '\n' || toParse[index] == '\t' || toParse[index] == ' ')
		++index;
	if (index == toParse.size())
	{
		return Token(-1, "END");
	}
	Token ret;
	if (isalpha(toParse[index]))
	{
		while (index < toParse.size() && isalnum(toParse[index]))
		{
			ret.value += toParse[index];
			++index;
		}
		while(isFilePathChar(toParse[index]))
		{
			ret.value += toParse[index];
			++index;
		}
		return ret;
	}
	if (toParse[index] == '-')
	{
		ret.value = "-";
		++index;
	}
	if (isdigit(toParse[index]))
	{
		while (index < toParse.size() && isdigit(toParse[index]))
		{
			ret.value += toParse[index];
			++index;
		}
		if (index < toParse.size() && toParse[index] == '.')
		{
			++index;
			ret.value += '.';
			bool legal = false;
			while (index < toParse.size() && isdigit(toParse[index]))
			{
				legal = true;
				ret.value += toParse[index];
				++index;
			}
			if (!legal)
			{
				ret.tag = -1;
				return ret;
			}
		}
		return ret;
	}
	if (toParse[index] == '=')
	{
		ret.value = "=";
		++index;
		return ret;
	}
	return ret;
}
void Parser::Move()
{
	currentToken = GetNextToken();
}
bool Parser::Match(int _tag)
{
	if (currentToken.tag == _tag)
	{
		Move();
		return true;
	}
	throw exception("syntax error!");
	return false;
}
bool Parser::Match(const string& pattern)
{
	if (currentToken.value == pattern)
	{
		Move();
		return true;
	}
	return false;
}


Vector3 Parser::vec3()
{
	Vector3 ret;
	ret.x = stod(currentToken.value);
	Move();
	ret.y = stod(currentToken.value);
	Move();
	ret.z = stod(currentToken.value);
	Move();
	return ret;
}

shared_ptr<Scene> Parser::scene()
{
	shared_ptr<Scene> mainScene(new Scene());
	Move();
	mainScene->SetCamera(camera());
	while (!Finished())
	{
		if (currentToken.value == "AreaLight" || currentToken.value == "PointLight")
			mainScene->AddLight(light());
		else if (currentToken.value == "Sphere" || currentToken.value == "Plane" || currentToken.value == "Mesh")
			mainScene->AddObject(primitive());
	}
	return mainScene;
}

shared_ptr<Camera> Parser::camera()
{
	Match("camera");
	shared_ptr<Camera> ret(new Camera());
	Match("EyePosition");
	Match("=");
	ret->SetEyePosition(vec3());
	Match("LookAt");
	Match("=");
	ret->SetLookAt(vec3());
	Match("LensWidth");
	Match("=");
	ret->SetLensW(stof(currentToken.value));
	Move();

	Match("LensHeight");
	Match("=");
	ret->SetLensH(stof(currentToken.value));
	Move();
	
	if (currentToken.value=="OutFileName")
	{
		Move();
		Match("=");
		bool isFile= regex_match(currentToken.value, Parser::fileRegex);
		if (isFile)
			ret->SetOutFile(currentToken.value);
		else
			ret->SetOutFile("result.bmp");
		Move();
	}
	ret->Update();
	return ret;
}
shared_ptr<Light> Parser::light()
{
	if (currentToken.value == "AreaLight")
	{
		return arealgiht();
	}
	else if (currentToken.value == "PointLight")
		return nullptr;
	return nullptr;
}

shared_ptr<AreaLight> Parser::arealgiht()
{
	shared_ptr<AreaLight> ret(new AreaLight());
	Match("AreaLight");
	Match("center");
	Match("=");
	ret->SetCenter(vec3());
	Match("dx");
	Match("=");
	ret->SetDx(vec3());
	Match("dy");
	Match("=");
	ret->SetDy(vec3());
	
	Match("color");
	Match("=");
	ret->SetColor(vec3());
	return ret;
}

shared_ptr<Primitive> Parser::primitive()
{
	if (currentToken.value == "Sphere")
		return sphere();
	else if (currentToken.value == "Mesh")
		return mesh();
	return nullptr;
}
Material  Parser::material()
{
	Material ret;
	Match("color");
	Match("=");
	ret.color = vec3();

	Match("diff");
	Match("=");
	ret.diff = stof(currentToken.value);
	Move();

	Match("refl");
	Match("=");
	ret.refl = stof(currentToken.value);
	Move();
	Match("refr");
	Match("=");
	ret.refr = stof(currentToken.value);
	Move();

	if (currentToken.value=="rindex")
	{
		Match("rindex");
		Match("=");
		ret.rindex = stof(currentToken.value);
		Move();
	}
	if (currentToken.value == "texture")
	{
		Move();
		Match("=");
		ret.texture=make_shared<Bmp>();
		ret.texture->Input(currentToken.value);
		Move();
	}
	return ret;
}

shared_ptr<Sphere> Parser::sphere()
{
	shared_ptr<Sphere> ret(new Sphere());
	Match("Sphere");
	Match("center");
	Match("=");
	ret->SetCenter(vec3());
	Match("R");
	Match("=");
	ret->SetRadius(stof(currentToken.value));
	Move();
	ret->SetMaterial(material());
	return ret;
}

shared_ptr<Polyhedron> Parser::mesh()
{
	Match("Mesh");
	string filepath;
	Match("obj");
	Match("=");
	filepath = currentToken.value;
	Move();
	Match("rotation");
	Match("=");
	Vector3 rotaion = vec3();

	Match("offset");
	Match("=");
	Vector3 offset = vec3();

	Match("scale");
	Match("=");
	double scale = stod(currentToken.value);
	Move();
	shared_ptr<Polyhedron> poly(new Polyhedron(filepath, rotaion, offset, scale));
	poly->SetMaterial(material());
	return poly;
}