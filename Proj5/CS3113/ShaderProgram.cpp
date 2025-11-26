#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : mIsLoaded{false}
{
    mShader.id = NOT_LOADED;
}

ShaderProgram::~ShaderProgram()
{
    unload();
}

bool ShaderProgram::load(const std::string &vertexPath, const std::string &fragmentPath)
{
    mShader = LoadShader(vertexPath.c_str(), fragmentPath.c_str());
    
    if (mShader.id == NOT_LOADED) {
        LOG("Failed to load shader from " + vertexPath + " and " + fragmentPath);
        mIsLoaded = false;
        return false;
    }
    
    mIsLoaded = true;
    return true;
}

void ShaderProgram::unload()
{
    if (mIsLoaded) {
        UnloadShader(mShader);
        mShader.id = NOT_LOADED;
        mIsLoaded = false;
    }
}

void ShaderProgram::begin()
{
    if (mIsLoaded) {
        BeginShaderMode(mShader);
    }
}

void ShaderProgram::end()
{
    if (mIsLoaded) {
        EndShaderMode();
    }
}

void ShaderProgram::setVector2(const std::string &name, const Vector2 &value)
{
    if (!mIsLoaded) return;
    
    int location = GetShaderLocation(mShader, name.c_str());
    SetShaderValue(mShader, location, &value, SHADER_UNIFORM_VEC2);
}

void ShaderProgram::setFloat(const std::string &name, float value)
{
    if (!mIsLoaded) return;
    
    int location = GetShaderLocation(mShader, name.c_str());
    SetShaderValue(mShader, location, &value, SHADER_UNIFORM_FLOAT);
}

void ShaderProgram::setInt(const std::string &name, int value)
{
    if (!mIsLoaded) return;
    
    int location = GetShaderLocation(mShader, name.c_str());
    SetShaderValue(mShader, location, &value, SHADER_UNIFORM_INT);
}
