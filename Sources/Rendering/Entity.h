#pragma once
#include <glm/glm.hpp>
#include <string>

class Model;
class Shader;
class Entity
{
public:
   Entity(const std::string_view name) :
   m_name(name),
   m_transform(glm::mat4()),
   m_model(nullptr)
   {
   }

   Entity(const Entity& rhs) = default;
   Entity(Entity&& rhs) noexcept :
      m_transform(rhs.m_transform),
      m_model(rhs.m_model)
   {
      rhs.m_model = nullptr;
      rhs.m_transform = glm::mat4();
   }

   Entity& operator=(const Entity& rhs) = default;
   Entity& operator=(Entity&& rhs) noexcept
   {
      m_transform = rhs.m_transform;
      m_model = rhs.m_model;
      rhs.m_model = nullptr;
      rhs.m_transform = glm::mat4();

      return (*this);
   }

   ~Entity() = default;

   void SetModel(Model* model) { m_model = model; }

   [[nodiscard]]
   Model* GetModel() const { return m_model; }

   void SetTransform(const glm::mat4& transform) { m_transform = transform; }

   [[nodiscard]]
   glm::mat4 GetTransform() const { return m_transform; }

   void Render(Shader& shader) const;

private:
   std::string m_name;
   glm::mat4 m_transform;
   Model* m_model;

};