#pragma once
#include <glm/glm.hpp>

class Light
{
public:
   Light(const glm::vec3& position, const glm::vec3& color) :
   m_position(position),
   m_color(color)
   {
   }

   Light(const Light& rhs) = default;
   Light(Light&& rhs) noexcept :
   Light(rhs.m_position, rhs.m_color)
   {
      rhs.m_position = glm::vec3();
      rhs.m_color = glm::vec3(1.0);
   }

   Light() :
   Light(glm::vec3(), glm::vec3(1.0))
   {
   }

   ~Light() = default;

   Light& operator=(const Light&) = default;
   Light& operator=(Light&& rhs) noexcept
   {
      m_position = rhs.m_position;
      m_color = rhs.m_color;
      rhs.m_position = glm::vec3();
      rhs.m_color = glm::vec3(1.0f);
      return (*this);
   }

   [[nodiscard]]
   glm::vec3 GetPosition() const { return m_position; }
   void SetPosition(const glm::vec3& newPos) { m_position = newPos; }

   [[nodiscard]]
   glm::vec3 GetColor() const { return m_color; }
   void SetColor(const glm::vec3& newColor) { m_color = newColor; }

private:
   glm::vec3 m_position;
   glm::vec3 m_color;

};
