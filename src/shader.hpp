#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>

#include <string>

class Shader {
  private:
    unsigned int ID;

  public:
    Shader(const char *vertexPath, const char *fragmentPath);

    Shader(const Shader &) = delete;
    auto operator = (const Shader &) -> Shader & = delete;
    Shader(Shader &&) = delete;
    auto operator = (Shader &&) -> Shader & = delete;

    virtual~Shader();

    static void checkCompileErrors(unsigned int shader, const std::string &type);

    void use() const;

    auto getID() const -> unsigned int;

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};

#endif
