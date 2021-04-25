#pragma once
#include <vector>
#include <string>

class Entity;
class Light;
class Scene
{
public:
   Scene() = default;
   virtual ~Scene();

   Entity& CreateEntity(const std::string_view name);
   Light& CreateLight();

   [[nodiscard]]
   const std::vector<Entity*>& GetEntities() const { return m_entities; }
   [[nodiscard]]
   const std::vector<Light*>& GetLights() const { return m_lights; }

private:
   std::vector<Entity*> m_entities;
   std::vector<Light*> m_lights;

};