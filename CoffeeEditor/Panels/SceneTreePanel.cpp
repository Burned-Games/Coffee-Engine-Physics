#include "SceneTreePanel.h"

#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Core/FileDialog.h"
#include "CoffeeEngine/IO/Resource.h"
#include "CoffeeEngine/Project/Project.h"
#include "CoffeeEngine/Renderer/Camera.h"
#include "CoffeeEngine/Renderer/Material.h"
#include "CoffeeEngine/Renderer/Texture.h"
#include "CoffeeEngine/Scene/Components.h"
#include "CoffeeEngine/Scene/Entity.h"
#include "CoffeeEngine/Scene/PrimitiveMesh.h"
#include "CoffeeEngine/Scene/Scene.h"
#include "CoffeeEngine/Scene/SceneCamera.h"
#include "CoffeeEngine/Scene/SceneTree.h"
#include "CoffeeEngine/Scripting/Lua/LuaBackend.h"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "imgui_internal.h"
#include <IconsLucide.h>

#include <CoffeeEngine/Scripting/Script.h>
#include <array>
#include <cstdint>
#include <cstring>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <string>

namespace Coffee
{

    SceneTreePanel::SceneTreePanel(const Ref<Scene>& scene)
    {
        m_Context = scene;
    }

    void SceneTreePanel::SetContext(const Ref<Scene>& scene)
    {
        m_Context = scene;
    }

    void SceneTreePanel::OnImGuiRender()
    {
        if (!m_Visible)
            return;

        ImGui::Begin("Scene Tree");

        // delete node and all children if supr is pressed and the node is selected
        if (ImGui::IsKeyPressed(ImGuiKey_Delete) && m_SelectionContext)
        {
            m_Context->DestroyEntity(m_SelectionContext);
            m_SelectionContext = {};
        }

        // Button for adding entities to the scene tree
        if (ImGui::Button(ICON_LC_PLUS, {24, 24}))
        {
            ImGui::OpenPopup("Add Entity...");
        }
        ShowCreateEntityMenu();
        ImGui::SameLine();

        static std::array<char, 256> searchBuffer;
        ImGui::InputTextWithHint("##searchbar", ICON_LC_SEARCH " Search by name:", searchBuffer.data(),
                                 searchBuffer.size());

        ImGui::BeginChild("entity tree", {0, 0}, ImGuiChildFlags_Border);

        auto view = m_Context->m_Registry.view<entt::entity>();
        for (auto entityID : view)
        {
            Entity entity{entityID, m_Context.get()};
            auto& hierarchyComponent = entity.GetComponent<HierarchyComponent>();

            if (hierarchyComponent.m_Parent == entt::null)
            {
                DrawEntityNode(entity);
            }
        }

        ImGui::EndChild();

        // Entity Tree Drag and Drop functionality
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE"))
            {
                const Ref<Resource>& resource = *(Ref<Resource>*)payload->Data;
                switch (resource->GetType())
                {
                case ResourceType::Model: {
                    const Ref<Model>& model = std::static_pointer_cast<Model>(resource);
                    AddModelToTheSceneTree(m_Context.get(), model);
                    break;
                }
                default:
                    break;
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            m_SelectionContext = {};
        }

        ImGui::End();

        ImGui::Begin("Inspector");
        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);
        }

        ImGui::End();
    }

    void SceneTreePanel::DrawEntityNode(Entity entity)
    {
        auto& entityNameTag = entity.GetComponent<TagComponent>().Tag;

        auto& hierarchyComponent = entity.GetComponent<HierarchyComponent>();

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
                                   ((hierarchyComponent.m_First == entt::null) ? ImGuiTreeNodeFlags_Leaf : 0) |
                                   ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding |
                                   ImGuiTreeNodeFlags_SpanAvailWidth;

        bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entityNameTag.c_str());

        if (ImGui::IsItemClicked())
        {
            m_SelectionContext = entity;
        }

        // Code of Double clicking the item for changing the name (WIP)

        ImVec2 itemSize = ImGui::GetItemRectSize();

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            ImVec2 popupPos = ImGui::GetItemRectMin();
            float indent = ImGui::GetStyle().IndentSpacing;
            ImGui::SetNextWindowPos({popupPos.x + indent, popupPos.y});
            ImGui::OpenPopup("EntityPopup");
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        if (ImGui::BeginPopup("EntityPopup" /*, ImGuiWindowFlags_NoBackground*/))
        {
            auto buff = entity.GetComponent<TagComponent>().Tag.c_str();
            ImGui::SetNextItemWidth(itemSize.x - ImGui::GetStyle().IndentSpacing);
            ImGui::InputText("##entity-name", (char*)buff, 128);
            ImGui::EndPopup();
        }

        ImGui::PopStyleVar();

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload("ENTITY_NODE", &entity,
                                      sizeof(Entity)); // Use the entity ID or a pointer as payload
            ImGui::Text("%s", entityNameTag.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_NODE"))
            {
                // Assuming payload is an Entity, but you need to cast and check appropriately
                Entity payloadEntity = *(const Entity*)payload->Data;
                // Process the drop, e.g., reparenting the entity in the hierarchy
                // This is where you would update the ECS or scene graph
                HierarchyComponent::Reparent(
                    m_Context->m_Registry, (entt::entity)payloadEntity,
                    entity); // I think is not necessary do the casting, it does it automatically;
            }
            ImGui::EndDragDropTarget();
        }

        if (opened)
        {
            if (hierarchyComponent.m_First != entt::null)
            {
                // Recursively draw all children
                Entity childEntity{hierarchyComponent.m_First, m_Context.get()};
                while ((entt::entity)childEntity != entt::null)
                {
                    DrawEntityNode(childEntity);
                    auto& childHierarchyComponent = childEntity.GetComponent<HierarchyComponent>();
                    childEntity = Entity{childHierarchyComponent.m_Next, m_Context.get()};
                }
            }
            ImGui::TreePop();
        }
    }
    bool setshape = false;
    void SceneTreePanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())
        {
            auto& entityNameTag = entity.GetComponent<TagComponent>().Tag;

            ImGui::Text(ICON_LC_TAG " Tag");
            ImGui::SameLine();

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, entityNameTag.c_str());

            if (ImGui::InputText("##", buffer, sizeof(buffer)))
            {
                entityNameTag = std::string(buffer);
            }

            ImGui::Separator();
        }

        if (entity.HasComponent<TransformComponent>())
        {
            auto& transformComponent = entity.GetComponent<TransformComponent>();

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Position");
                ImGui::DragFloat3("##Position", glm::value_ptr(transformComponent.Position), 0.1f);

                ImGui::Text("Rotation");
                ImGui::DragFloat3("##Rotation", glm::value_ptr(transformComponent.Rotation), 0.1f);

                ImGui::Text("Scale");
                ImGui::DragFloat3("##Scale", glm::value_ptr(transformComponent.Scale), 0.1f);
            }
        }

        if (entity.HasComponent<CameraComponent>())
        {
            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            SceneCamera& sceneCamera = cameraComponent.Camera;
            bool isCollapsingHeaderOpen = true;
            if (ImGui::CollapsingHeader("Camera", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Projection Type");
                if (ImGui::BeginCombo("##Projection Type",
                                      sceneCamera.GetProjectionType() == Camera::ProjectionType::PERSPECTIVE
                                          ? "Perspective"
                                          : "Orthographic"))
                {
                    if (ImGui::Selectable("Perspective",
                                          sceneCamera.GetProjectionType() == Camera::ProjectionType::PERSPECTIVE))
                    {
                        sceneCamera.SetProjectionType(Camera::ProjectionType::PERSPECTIVE);
                    }
                    if (ImGui::Selectable("Orthographic",
                                          sceneCamera.GetProjectionType() == Camera::ProjectionType::ORTHOGRAPHIC))
                    {
                        sceneCamera.SetProjectionType(Camera::ProjectionType::ORTHOGRAPHIC);
                    }
                    ImGui::EndCombo();
                }

                if (sceneCamera.GetProjectionType() == Camera::ProjectionType::PERSPECTIVE)
                {
                    ImGui::Text("Field of View");
                    float fov = sceneCamera.GetFOV();
                    if (ImGui::DragFloat("##Field of View", &fov, 0.1f, 0.0f, 180.0f))
                    {
                        sceneCamera.SetFOV(fov);
                    }

                    ImGui::Text("Near Clip");
                    float nearClip = sceneCamera.GetNearClip();
                    if (ImGui::DragFloat("##Near Clip", &nearClip, 0.1f))
                    {
                        sceneCamera.SetNearClip(nearClip);
                    }

                    ImGui::Text("Far Clip");
                    float farClip = sceneCamera.GetFarClip();
                    if (ImGui::DragFloat("##Far Clip", &farClip, 0.1f))
                    {
                        sceneCamera.SetFarClip(farClip);
                    }
                }

                if (sceneCamera.GetProjectionType() == Camera::ProjectionType::ORTHOGRAPHIC)
                {
                    ImGui::Text("Orthographic Size");
                    float orthoSize = sceneCamera.GetFOV();
                    if (ImGui::DragFloat("##Orthographic Size", &orthoSize, 0.1f))
                    {
                        sceneCamera.SetFOV(orthoSize);
                    }

                    ImGui::Text("Near Clip");
                    float nearClip = sceneCamera.GetNearClip();
                    if (ImGui::DragFloat("##Near Clip", &nearClip, 0.1f))
                    {
                        sceneCamera.SetNearClip(nearClip);
                    }

                    ImGui::Text("Far Clip");
                    float farClip = sceneCamera.GetFarClip();
                    if (ImGui::DragFloat("##Far Clip", &farClip, 0.1f))
                    {
                        sceneCamera.SetFarClip(farClip);
                    }
                }

                ImGui::Text("Sphere Collider");

                if (entity.HasComponent<SphereColliderComponent>())
                {
                    if (ImGui::Button("Remove Sphere Collider"))
                    {
                        entity.RemoveComponent<SphereColliderComponent>();
                    }
                }
                else
                {
                    if (ImGui::Button("Add Sphere Collider"))
                    {
                        entity.AddComponent<SphereColliderComponent>();
                    }
                }

                if (!isCollapsingHeaderOpen)
                {
                    entity.RemoveComponent<CameraComponent>();
                }
            }
        }

        if (entity.HasComponent<LightComponent>())
        {
            auto& lightComponent = entity.GetComponent<LightComponent>();
            bool isCollapsingHeaderOpen = true;
            if (ImGui::CollapsingHeader("Light", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Light Type");
                ImGui::Combo("##Light Type", (int*)&lightComponent.type, "Directional\0Point\0Spot\0");

                ImGui::Text("Color");
                ImGui::ColorEdit3("##Color", glm::value_ptr(lightComponent.Color));

                ImGui::Text("Intensity");
                ImGui::DragFloat("##Intensity", &lightComponent.Intensity, 0.1f);

                if (lightComponent.type == LightComponent::Type::PointLight ||
                    lightComponent.type == LightComponent::Type::SpotLight)
                {
                    ImGui::Text("Range");
                    ImGui::DragFloat("##Range", &lightComponent.Range, 0.1f);
                }

                if (lightComponent.type == LightComponent::Type::PointLight)
                {
                    ImGui::Text("Attenuation");
                    ImGui::DragFloat("##Attenuation", &lightComponent.Attenuation, 0.1f);
                }
                if (!isCollapsingHeaderOpen)
                {
                    entity.RemoveComponent<LightComponent>();
                }
            }
        }

        if (entity.HasComponent<MeshComponent>())
        {
            auto& meshComponent = entity.GetComponent<MeshComponent>();
            bool isCollapsingHeaderOpen = true;
            if (ImGui::CollapsingHeader("Mesh", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Mesh");
                ImGui::SameLine();
                if (ImGui::Button(meshComponent.GetMesh()->GetName().c_str(), {64, 32}))
                {
                    ImGui::OpenPopup("MeshPopup");
                }
                if (ImGui::BeginPopup("MeshPopup"))
                {
                    if (ImGui::MenuItem("Quad"))
                    {
                        meshComponent.mesh = PrimitiveMesh::CreateQuad();
                    }
                    if (ImGui::MenuItem("Cube"))
                    {
                        meshComponent.mesh = PrimitiveMesh::CreateCube();
                    }
                    if (ImGui::MenuItem("Sphere"))
                    {
                        meshComponent.mesh = PrimitiveMesh::CreateSphere();
                    }
                    if (ImGui::MenuItem("Plane"))
                    {
                        meshComponent.mesh = PrimitiveMesh::CreatePlane();
                    }
                    if (ImGui::MenuItem("Cylinder"))
                    {
                        meshComponent.mesh = PrimitiveMesh::CreateCylinder();
                    }
                    if (ImGui::MenuItem("Cone"))
                    {
                        meshComponent.mesh = PrimitiveMesh::CreateCone();
                    }
                    if (ImGui::MenuItem("Torus"))
                    {
                        meshComponent.mesh = PrimitiveMesh::CreateTorus();
                    }
                    if (ImGui::MenuItem("Capsule"))
                    {
                        meshComponent.mesh = PrimitiveMesh::CreateCapsule();
                    }
                    if (ImGui::MenuItem("Save Mesh"))
                    {
                        COFFEE_ERROR("Save Mesh not implemented yet!");
                    }
                    ImGui::EndPopup();
                }
                ImGui::Checkbox("Draw AABB", &meshComponent.drawAABB);

                if (!isCollapsingHeaderOpen)
                {
                    entity.RemoveComponent<MeshComponent>();
                }
            }
        }

        if (entity.HasComponent<MaterialComponent>())
        {
            // Move this function to another site
            auto DrawTextureWidget = [&](const std::string& label, Ref<Texture2D>& texture) {
                auto& materialComponent = entity.GetComponent<MaterialComponent>();
                uint32_t textureID = texture ? texture->GetID() : 0;
                ImGui::ImageButton(label.c_str(), (ImTextureID)textureID, {64, 64});

                auto textureImageFormat = [](ImageFormat format) -> std::string {
                    switch (format)
                    {
                    case ImageFormat::R8:
                        return "R8";
                    case ImageFormat::RGB8:
                        return "RGB8";
                    case ImageFormat::RGBA8:
                        return "RGBA8";
                    case ImageFormat::SRGB8:
                        return "SRGB8";
                    case ImageFormat::SRGBA8:
                        return "SRGBA8";
                    case ImageFormat::RGBA32F:
                        return "RGBA32F";
                    case ImageFormat::DEPTH24STENCIL8:
                        return "DEPTH24STENCIL8";
                    }
                };

                if (ImGui::IsItemHovered() and texture)
                {
                    ImGui::SetTooltip("Name: %s\nSize: %d x %d\nPath: %s", texture->GetName().c_str(),
                                      texture->GetWidth(), texture->GetHeight(),
                                      textureImageFormat(texture->GetImageFormat()).c_str(),
                                      texture->GetPath().c_str());
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE"))
                    {
                        const Ref<Resource>& resource = *(Ref<Resource>*)payload->Data;
                        if (resource->GetType() == ResourceType::Texture2D)
                        {
                            const Ref<Texture2D>& t = std::static_pointer_cast<Texture2D>(resource);
                            texture = t;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::SameLine();
                if (ImGui::BeginCombo((label + "texture").c_str(), "", ImGuiComboFlags_NoPreview))
                {
                    if (ImGui::Selectable("Clear"))
                    {
                        texture = nullptr;
                    }
                    if (ImGui::Selectable("Open"))
                    {
                        std::string path = FileDialog::OpenFile({}).string();
                        if (!path.empty())
                        {
                            Ref<Texture2D> t = Texture2D::Load(path);
                            texture = t;
                        }
                    }
                    ImGui::EndCombo();
                }
            };
            auto DrawCustomColorEdit4 = [&](const std::string& label, glm::vec4& color,
                                            const glm::vec2& size = {100, 32}) {
                // ImGui::ColorEdit4("##Albedo Color", glm::value_ptr(materialProperties.color),
                // ImGuiColorEditFlags_NoInputs);
                if (ImGui::ColorButton(label.c_str(), ImVec4(color.r, color.g, color.b, color.a), NULL,
                                       {size.x, size.y}))
                {
                    ImGui::OpenPopup("AlbedoColorPopup");
                }
                if (ImGui::BeginPopup("AlbedoColorPopup"))
                {
                    ImGui::ColorPicker4((label + "Picker").c_str(), glm::value_ptr(color),
                                        ImGuiColorEditFlags_NoInputs);
                    ImGui::EndPopup();
                }
            };

            auto& materialComponent = entity.GetComponent<MaterialComponent>();
            bool isCollapsingHeaderOpen = true;
            if (ImGui::CollapsingHeader("Material", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                MaterialTextures& materialTextures = materialComponent.material->GetMaterialTextures();
                MaterialProperties& materialProperties = materialComponent.material->GetMaterialProperties();

                if (ImGui::TreeNode("Albedo"))
                {
                    ImGui::BeginChild("##Albedo Child", {0, 0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);

                    ImGui::Text("Color");
                    DrawCustomColorEdit4("##Albedo Color", materialProperties.color);

                    ImGui::Text("Texture");
                    DrawTextureWidget("##Albedo", materialTextures.albedo);

                    ImGui::EndChild();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Metallic"))
                {
                    ImGui::BeginChild("##Metallic Child", {0, 0},
                                      ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);
                    ImGui::Text("Metallic");
                    ImGui::SliderFloat("##Metallic Slider", &materialProperties.metallic, 0.0f, 1.0f);
                    ImGui::Text("Texture");
                    DrawTextureWidget("##Metallic", materialTextures.metallic);
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Roughness"))
                {
                    ImGui::BeginChild("##Roughness Child", {0, 0},
                                      ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);
                    ImGui::Text("Roughness");
                    ImGui::SliderFloat("##Roughness Slider", &materialProperties.roughness, 0.1f, 1.0f);
                    ImGui::Text("Texture");
                    DrawTextureWidget("##Roughness", materialTextures.roughness);
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Emission"))
                {
                    ImGui::BeginChild("##Emission Child", {0, 0},
                                      ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);
                    // FIXME: Emissive color variable is local and do not affect the materialProperties.emissive!!
                    glm::vec4& emissiveColor = reinterpret_cast<glm::vec4&>(materialProperties.emissive);
                    emissiveColor.a = 1.0f;
                    DrawCustomColorEdit4("Color", emissiveColor);
                    ImGui::Text("Texture");
                    DrawTextureWidget("##Emissive", materialTextures.emissive);
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Normal Map"))
                {
                    ImGui::BeginChild("##Normal Child", {0, 0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);
                    ImGui::Text("Texture");
                    DrawTextureWidget("##Normal", materialTextures.normal);
                    ImGui::EndChild();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Ambient Occlusion"))
                {
                    ImGui::BeginChild("##AO Child", {0, 0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);
                    ImGui::Text("AO");
                    ImGui::SliderFloat("##AO Slider", &materialProperties.ao, 0.0f, 1.0f);
                    ImGui::Text("Texture");
                    DrawTextureWidget("##AO", materialTextures.ao);
                    ImGui::EndChild();
                    ImGui::TreePop();
                }

                if (!isCollapsingHeaderOpen)
                {
                    entity.RemoveComponent<MaterialComponent>();
                }
            }
        }

        if (entity.HasComponent<ScriptComponent>())
        {
            auto& scriptComponent = entity.GetComponent<ScriptComponent>();
            bool isCollapsingHeaderOpen = true;
            if (ImGui::CollapsingHeader("Script", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                /*
                ImGui::Text("Script Name: ");
                ImGui::Text(scriptComponent.script.GetLanguage() == ScriptingLanguage::Lua ? "Lua" : "CSharp");

                ImGui::Text("Script Path: ");
                ImGui::Text(scriptComponent.script.GetPath().string().c_str());
                */

                // Get the exposed variables
                std::vector<LuaVariable> exposedVariables =
                    LuaBackend::MapVariables(scriptComponent.script.GetPath().string());

                // print the exposed variables
                for (auto& variable : exposedVariables)
                {
                    auto it = LuaBackend::scriptEnvironments.find(scriptComponent.script.GetPath().string());
                    if (it == LuaBackend::scriptEnvironments.end())
                    {
                        COFFEE_CORE_ERROR("Script environment for {0} not found",
                                          scriptComponent.script.GetPath().string());
                        continue;
                    }

                    sol::environment& env = it->second;

                    switch (variable.type)
                    {
                    case sol::type::boolean: {
                        bool value = env[variable.name];
                        if (ImGui::Checkbox(variable.name.c_str(), &value))
                        {
                            env[variable.name] = value;
                        }
                        break;
                    }
                    case sol::type::number: {
                        float number = env[variable.name];
                        if (ImGui::InputFloat(variable.name.c_str(), &number))
                        {
                            env[variable.name] = number;
                        }
                        break;
                    }
                    case sol::type::string: {
                        std::string str = env[variable.name];
                        char buffer[256];
                        memset(buffer, 0, sizeof(buffer));
                        strcpy(buffer, str.c_str());

                        if (ImGui::InputText(variable.name.c_str(), buffer, sizeof(buffer)))
                        {
                            env[variable.name] = std::string(buffer);
                        }
                        break;
                    }
                    case sol::type::none: {
                        ImGui::SeparatorText(variable.value.c_str());
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        }
        

        if (entity.HasComponent<RigidbodyComponent>())
        {
            
            auto& rigidbodyComponent = entity.GetComponent<RigidbodyComponent>();
            bool isCollapsingHeaderOpen = true;

            if (ImGui::CollapsingHeader("Rigidbody", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // RigidBody type
                const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
                int currentType = static_cast<int>(rigidbodyComponent.cfg.type);
                if (ImGui::Combo("Body Type", &currentType, bodyTypeStrings, 3))
                {
                    rigidbodyComponent.cfg.type = static_cast<RigidBodyType>(currentType);
                    
                    // Actualizar configuración basada en el tipo
                    if (rigidbodyComponent.cfg.type == RigidBodyType::Static)
                    {
                        rigidbodyComponent.cfg.UseGravity = false;
                    }
                    else if (rigidbodyComponent.cfg.type == RigidBodyType::Kinematic)
                    {
                        rigidbodyComponent.cfg.UseGravity = false;
                        rigidbodyComponent.cfg.Velocity = glm::vec3(0.0f);
                    }
                }

                // Use Gravity checkbox (only for Dynamic)
                if (rigidbodyComponent.cfg.type == RigidBodyType::Dynamic && !rigidbodyComponent.cfg.FreezeY)
                {
                    ImGui::Checkbox("Use Gravity", &rigidbodyComponent.cfg.UseGravity);
                }

                // Mass
                ImGui::Text("Mass");
                ImGui::DragFloat("##Mass", &rigidbodyComponent.cfg.shapeConfig.mass, 0.1f, 0.001f, 0.0f);

                // Drag
                ImGui::Text("Linear Drag");
                ImGui::DragFloat("##Linear Drag", &rigidbodyComponent.cfg.LinearDrag, 0.1f, 0.001f, 10.0f,
                                 "Linear Drag: %.3f");
                // Modify Angular Drag
                ImGui::Text("Angular Drag");
                ImGui::DragFloat("##Angular Drag", &rigidbodyComponent.cfg.AngularDrag, 0.1f, 0.001f, 10.0f,
                                 "Angular Drag: %.3f");
                
                // Friction
                ImGui::Text("Friction");
                if (ImGui::DragFloat("##Friction", &rigidbodyComponent.cfg.friction, 0.05f, 0.0f, 1.0f, 
                                    "Friction: %.2f"))
                {
                    if (rigidbodyComponent.m_RigidBody)
                        rigidbodyComponent.m_RigidBody->SetFriction(rigidbodyComponent.cfg.friction);
                }
                
                // Restitution (Bounce)
                ImGui::Text("Restitution (Bounce)");
                if (ImGui::DragFloat("##Restitution", &rigidbodyComponent.cfg.restitution, 0.05f, 0.0f, 1.0f, 
                                    "Restitution: %.2f"))
                {
                    if (rigidbodyComponent.m_RigidBody)
                        rigidbodyComponent.m_RigidBody->SetRestitution(rigidbodyComponent.cfg.restitution);
                }

                // Velocity
                ImGui::Text("Velocity");
                ImGui::DragFloat3("##Velocity", glm::value_ptr(rigidbodyComponent.cfg.Velocity), 0.1f);
                //COFFEE_CORE_INFO("Velocity: (%f, %f, %f)", rigidbodyComponent.cfg.Velocity.x, rigidbodyComponent.cfg.Velocity.y,
                //                 rigidbodyComponent.cfg.Velocity.z); 
                // Acceleration
                ImGui::Text("Acceleration");
                ImGui::DragFloat3("##Acceleration", glm::value_ptr(rigidbodyComponent.cfg.Acceleration), 0.1f);

                if (ImGui::CollapsingHeader("Constraints"))
                {
                    // Secci�n para Freeze de posici�n
                    ImGui::Text("Position");
                    ImGui::Indent();
                    ImGui::Checkbox("Freeze X", &rigidbodyComponent.cfg.FreezeX);
                    ImGui::Checkbox("Freeze Y", &rigidbodyComponent.cfg.FreezeY);
                    ImGui::Checkbox("Freeze Z", &rigidbodyComponent.cfg.FreezeZ);
                    ImGui::Unindent();

                    // Secci�n para Freeze de rotaci�n
                    ImGui::Text("Rotation");
                    ImGui::Indent();
                    ImGui::Checkbox("Freeze Rotation X", &rigidbodyComponent.cfg.FreezeRotX);
                    ImGui::Checkbox("Freeze Rotation Y", &rigidbodyComponent.cfg.FreezeRotY);
                    ImGui::Checkbox("Freeze Rotation Z", &rigidbodyComponent.cfg.FreezeRotZ);
                    ImGui::Unindent();
                }

                // Force Test Buttons Section
                if (ImGui::CollapsingHeader("Force Tests"))
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
                    
                    // Y-Axis Impulses
                    if (ImGui::Button("Impulse Up", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.ApplyImpulse(glm::vec3(0.0f, 5.0f, 0.0f));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Impulse Down", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.ApplyImpulse(glm::vec3(0.0f, -5.0f, 0.0f));
                    }

                    ImGui::Separator();

                    // Z-Axis Impulses
                    if (ImGui::Button("Impulse Forward", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.ApplyImpulse(glm::vec3(0.0f, 0.0f, 5.0f));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Impulse Back", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.ApplyImpulse(glm::vec3(0.0f, 0.0f, -5.0f));
                    }

                    ImGui::Separator();

                    // X-Axis Impulses
                    if (ImGui::Button("Impulse Right", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.ApplyImpulse(glm::vec3(5.0f, 0.0f, 0.0f));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Impulse Left", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.ApplyImpulse(glm::vec3(-5.0f, 0.0f, 0.0f));
                    }

                    ImGui::PopStyleVar();
                }

                // Velocity Test Buttons Section
                if (ImGui::CollapsingHeader("Velocity Tests"))
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 8));
                    
                    // Linear Velocity Tests
                    ImGui::Text("Linear Velocity");
                    
                    // Y-Axis Velocities
                    if (ImGui::Button("Vel Up", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetVelocity(glm::vec3(0.0f, 5.0f, 0.0f));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Vel Down", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetVelocity(glm::vec3(0.0f, -5.0f, 0.0f));
                    }

                    ImGui::Separator();

                    // Z-Axis Velocities
                    if (ImGui::Button("Vel Forward", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetVelocity(glm::vec3(0.0f, 0.0f, 5.0f));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Vel Back", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetVelocity(glm::vec3(0.0f, 0.0f, -5.0f));
                    }

                    ImGui::Separator();

                    // X-Axis Velocities 
                    if (ImGui::Button("Vel Right", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetVelocity(glm::vec3(5.0f, 0.0f, 0.0f));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Vel Left", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetVelocity(glm::vec3(-5.0f, 0.0f, 0.0f));
                    }

                    if (ImGui::Button("Apply Collider", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.ApplyShape(
                            entity.GetComponent<BoxColliderComponent>().m_Collider->GetShape(),
                            entity.GetComponent<BoxColliderComponent>().m_Collider->GetPosition(),
                            entity.GetComponent<BoxColliderComponent>().m_Collider->GetRotation(),
                            entity.GetComponent<BoxColliderComponent>().m_Collider->GetSize());
                    }

                    ImGui::Separator();

                    // Angular Velocity Tests
                    ImGui::Text("Angular Velocity");
                    
                    // Rotation around Y axis
                    if (ImGui::Button("Rotate Y+", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetAngularVelocity(glm::vec3(0.0f, 5.0f, 0.0f));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Rotate Y-", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetAngularVelocity(glm::vec3(0.0f, -5.0f, 0.0f));
                    }

                    // Rotation around X axis
                    if (ImGui::Button("Rotate X+", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetAngularVelocity(glm::vec3(5.0f, 0.0f, 0.0f));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Rotate X-", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetAngularVelocity(glm::vec3(-5.0f, 0.0f, 0.0f));
                    }

                    // Rotation around Z axis
                    if (ImGui::Button("Rotate Z+", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetAngularVelocity(glm::vec3(0.0f, 0.0f, 5.0f));
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Rotate Z-", ImVec2(120, 30)))
                    {
                        rigidbodyComponent.SetAngularVelocity(glm::vec3(0.0f, 0.0f, -5.0f));
                    }

                    ImGui::PopStyleVar();
                }
                /*if (entity.HasComponent<BoxColliderComponent>() && !setshape)
                {

                    rigidbodyComponent.ApplyShape(entity.GetComponent<BoxColliderComponent>().m_Collider->GetShape(),
                                                  entity.GetComponent<BoxColliderComponent>().m_Collider->GetPosition(),
                                                  entity.GetComponent<BoxColliderComponent>().m_Collider->GetRotation(),
                                                  entity.GetComponent<BoxColliderComponent>().m_Collider->GetSize());
                    setshape = true;
                }*/

            }

            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<RigidbodyComponent>();
            }
        }

       if (entity.HasComponent<BoxColliderComponent>())
        {
            auto& boxCollider = entity.GetComponent<BoxColliderComponent>();
            bool isCollapsingHeaderOpen = true;

            ImGui::PushID("BoxCollider"); // Unique ID
            if (ImGui::CollapsingHeader("Box Collider", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Size
                ImGui::Text("Size");
                if (ImGui::DragFloat3("##BoxSize", glm::value_ptr(boxCollider.Size), 0.1f, 0.0f, 100.0f))
                {
                    // Update collider size
                    //boxCollider.m_Collider->SetPosition(boxCollider.Offset);
                    /*boxCollider.m_Collider->ColliderUpdate(
                        entity.GetComponent<TransformComponent>().Position,
                        entity.GetComponent<TransformComponent>().Rotation,
                                                           boxCollider.Size);
                    printf("\n x:%f, y:%f, z:%f ", boxCollider.Size.x, boxCollider.Size.y, boxCollider.Size.z);*/
                }

                // Offset
                ImGui::Text("Offset");
                if (ImGui::DragFloat3("##BoxOffset", glm::value_ptr(boxCollider.Offset), 0.1f))
                {
                    // Update collider position
                    glm::vec3 offset = boxCollider.Offset;
                    //boxCollider.m_Collider->SetPosition(entity.GetComponent<TransformComponent>().Position, offset);

                   /* boxCollider.m_Collider->ColliderUpdate(entity.GetComponent<TransformComponent>().Position,
                                                           entity.GetComponent<TransformComponent>().Rotation,
                                                           boxCollider.Size, offset);*/
                }

                // Is Trigger
                if (ImGui::Checkbox("Is Trigger", &boxCollider.IsTrigger))
                {
                    // Update trigger state
                    boxCollider.m_Collider->SetEnabled(!boxCollider.IsTrigger);
                }

                // Mass
                ImGui::Text("Mass");
                ImGui::DragFloat("##Mass", &boxCollider.Mass, 0.1f, 0.001f, 1000.0f);

                // Material
                ImGui::Text("Material");
                ImGui::Combo("Material", &boxCollider.MaterialIndex, "None\0Physic Material\0\0");

                // Layer Overrides
                if (ImGui::TreeNode("Layer Overrides"))
                {
                    // Add properties for layer overrides here
                    ImGui::TreePop();
                }
          
                glm::vec3 offset = boxCollider.Offset;
                glm::mat4 transformMatrix = entity.GetComponent<TransformComponent>().GetLocalTransform();
                glm::quat rotation = glm::quat_cast(transformMatrix);

                glm::vec3 test = entity.GetComponent<TransformComponent>().Position;
                boxCollider.m_Collider->ColliderUpdate(entity.GetComponent<TransformComponent>().Position,
                                                       rotation, 
                                                       boxCollider.Size, offset);

            }

            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<BoxColliderComponent>();
            }
            ImGui::PopID(); // End Unique ID
        }

        if (entity.HasComponent<SphereColliderComponent>())
        {
            auto& sphereCollider = entity.GetComponent<SphereColliderComponent>();
            bool isCollapsingHeaderOpen = true;

            ImGui::PushID("SphereCollider"); // Unic ID
            if (ImGui::CollapsingHeader("Sphere Collider", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // center
                ImGui::Text("Center");
                ImGui::DragFloat3("##SphereCenter", glm::value_ptr(sphereCollider.Center), 0.1f, 0.0f, 100.0f);

                // radius
                ImGui::Text("Radius");
                ImGui::DragFloat("##SphereRadius", &sphereCollider.Radius, 0.1f, 0.0f, 100.0f);

                // is trigger
                ImGui::Checkbox("Is Trigger", &sphereCollider.IsTrigger);

                // provides contacts
                ImGui::Checkbox("Provides Contacts", &sphereCollider.ProvidesContacts);

                // material
                ImGui::Text("Material");
                ImGui::Combo("Material", &sphereCollider.MaterialIndex, "None\0Physic Material\0\0");
            }

            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<SphereColliderComponent>();
            }
            ImGui::PopID(); // end Unic ID
        }

        if (entity.HasComponent<CapsuleColliderComponent>())
        {
            auto& capsuleCollider = entity.GetComponent<CapsuleColliderComponent>();
            bool isCollapsingHeaderOpen = true;

            ImGui::PushID("CapsuleCollider"); // Unic ID
            if (ImGui::CollapsingHeader("Capsule Collider", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // center
                ImGui::Text("Center");
                ImGui::DragFloat3("##CapsuleCenter", glm::value_ptr(capsuleCollider.Center), 0.1f, 0.0f, 100.0f);

                // radius
                ImGui::Text("Radius");
                ImGui::DragFloat("##CapsuleRadius", &capsuleCollider.Radius, 0.1f, 0.0f, 100.0f);

                // height
                ImGui::Text("Height");
                ImGui::DragFloat("##CapsuleHeight", &capsuleCollider.Height, 0.1f, 0.0f, 100.0f);

                // direction
                ImGui::Text("Direction");
                ImGui::Combo("##CapsuleDirection", &capsuleCollider.DirectionIndex, "X-Axis\0Y-Axis\0Z-Axis\0\0");

                // is trigger
                ImGui::Checkbox("Is Trigger", &capsuleCollider.IsTrigger);

                // provides contacts
                ImGui::Checkbox("Provides Contacts", &capsuleCollider.ProvidesContacts);

                // material
                ImGui::Text("Material");
                ImGui::Combo("Material", &capsuleCollider.MaterialIndex, "None\0Physic Material\0\0");

                // Layer Overrides
                if (ImGui::TreeNode("Layer Overrides"))
                {
                    // Add properties for layer overrides here
                    ImGui::TreePop();
                }
            }

            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<CapsuleColliderComponent>();
            }
            ImGui::PopID(); // end Unic ID
        }

        if (entity.HasComponent<CylinderColliderComponent>())
            {
            auto& cylinderCollider = entity.GetComponent<CylinderColliderComponent>();
            bool isCollapsingHeaderOpen = true;

            ImGui::PushID("CylinderCollider"); // Unic ID
            if (ImGui::CollapsingHeader("Cylinder Collider", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // center
                ImGui::Text("Center");
                ImGui::DragFloat3("##CylinderCenter", glm::value_ptr(cylinderCollider.Center), 0.1f, 0.0f, 100.0f);

                // radius
                ImGui::Text("Radius");
                ImGui::DragFloat("##CylinderRadius", &cylinderCollider.Radius, 0.1f, 0.0f, 100.0f);

                // height
                ImGui::Text("Height");
                ImGui::DragFloat("##CylinderHeight", &cylinderCollider.Height, 0.1f, 0.0f, 100.0f);

                // direction
                ImGui::Text("Direction");
                ImGui::Combo("##CylinderDirection", &cylinderCollider.DirectionIndex, "X-Axis\0Y-Axis\0Z-Axis\0\0");

                // is trigger
                ImGui::Checkbox("Is Trigger", &cylinderCollider.IsTrigger);

                // provides contacts
                ImGui::Checkbox("Provides Contacts", &cylinderCollider.ProvidesContacts);

                // material
                ImGui::Text("Material");
                ImGui::Combo("Material", &cylinderCollider.MaterialIndex, "None\0Physic Material\0\0");

                // Layer Overrides
                if (ImGui::TreeNode("Layer Overrides"))
                {
                    // Add properties for layer overrides here
                    ImGui::TreePop();
                }
            }

            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<CylinderColliderComponent>();
            }
            ImGui::PopID(); // end Unic ID
        }

        if (entity.HasComponent<PlaneColliderComponent>())
        {
            auto& planeCollider = entity.GetComponent<PlaneColliderComponent>();
            bool isCollapsingHeaderOpen = true;

            ImGui::PushID("PlaneCollider"); // Unic ID
            if (ImGui::CollapsingHeader("Plane Collider", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // size
                ImGui::Text("Size");
                ImGui::DragFloat3("##PlaneSize", glm::value_ptr(planeCollider.Size), 0.1f, 0.0f, 100.0f);

                // offset
                ImGui::Text("Offset");
                ImGui::DragFloat3("##PlaneOffset", glm::value_ptr(planeCollider.Offset), 0.1f);

                // is trigger
                ImGui::Checkbox("Is Trigger", &planeCollider.IsTrigger);
            }

            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<PlaneColliderComponent>();
            }
            ImGui::PopID(); // end Unic ID
        }

        if (entity.HasComponent<MeshColliderComponent>())
        {
            auto& meshCollider = entity.GetComponent<MeshColliderComponent>();
            bool isCollapsingHeaderOpen = true;

            ImGui::PushID("MeshCollider"); // Unic ID
            if (ImGui::CollapsingHeader("Mesh Collider", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // size
                ImGui::Text("Size");
                ImGui::DragFloat3("##MeshSize", glm::value_ptr(meshCollider.Size), 0.1f, 0.0f, 100.0f);

                // offset
                ImGui::Text("Offset");
                ImGui::DragFloat3("##MeshOffset", glm::value_ptr(meshCollider.Offset), 0.1f);

                // is trigger
                ImGui::Checkbox("Is Trigger", &meshCollider.IsTrigger);

                // provides contacts
                ImGui::Checkbox("Provides Contacts", &meshCollider.ProvidesContacts);

                // cooking options
                ImGui::Text("Cooking Options");
                ImGui::Combo("##CookingOptions", &meshCollider.CookingOptionsIndex, "None\0Everything\0\0");

                // material
                ImGui::Text("Material");
                ImGui::Combo("Material", &meshCollider.MaterialIndex, "None\0Physic Material\0\0");

                // mesh
                ImGui::Text("Mesh");
                ImGui::Combo("Mesh", &meshCollider.MeshIndex, "None\0Mesh1\0Mesh2\0\0");

                // Layer Overrides
                if (ImGui::TreeNode("Layer Overrides"))
                {
                    // Add properties for layer overrides here
                    ImGui::TreePop();
                }
            }

            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<MeshColliderComponent>();
            }
            ImGui::PopID(); // end Unic ID
        }

        // Joint
        if (entity.HasComponent<FixedJointComponent>())
        {
            auto& fixedJoint = entity.GetComponent<FixedJointComponent>();
            bool isCollapsingHeaderOpen = true;

            ImGui::PushID("FixedJoint"); // Unique ID
            if (ImGui::CollapsingHeader("Fixed Joint", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Connected Body Field
                ImGui::Text("Connected Body");

                
                if (ImGui::Button(fixedJoint.ConnectedBody[0] != '\0' ? fixedJoint.ConnectedBody : "None (Rigidbody)",
                                  ImVec2(200, 20)))
                {
                  
                    strcpy(fixedJoint.ConnectedBody, ""); 
                }

               
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RIGIDBODY"))
                    {
                        const char* droppedObjectName = (const char*)payload->Data;
                        strncpy(fixedJoint.ConnectedBody, droppedObjectName, sizeof(fixedJoint.ConnectedBody) - 1);
                        fixedJoint.ConnectedBody[sizeof(fixedJoint.ConnectedBody) - 1] = '\0'; 
                    }
                    ImGui::EndDragDropTarget();
                }

                // Break Force
                ImGui::Text("Break Force");
                ImGui::DragFloat("##BreakForce", &fixedJoint.BreakForce, 1.0f, 0.0f, FLT_MAX);

                // Break Torque
                ImGui::Text("Break Torque");
                ImGui::DragFloat("##BreakTorque", &fixedJoint.BreakTorque, 1.0f, 0.0f, FLT_MAX);

                // Enable Collision
                ImGui::Checkbox("Enable Collision", &fixedJoint.EnableCollision);

                // Enable Preprocessing
                ImGui::Checkbox("Enable Preprocessing", &fixedJoint.EnablePreprocessing);

                // Mass Scale
                ImGui::Text("Mass Scale");
                ImGui::DragFloat("##MassScale", &fixedJoint.MassScale, 0.1f, 0.0f, 10.0f);

                // Connected Mass Scale
                ImGui::Text("Connected Mass Scale");
                ImGui::DragFloat("##ConnectedMassScale", &fixedJoint.ConnectedMassScale, 0.1f, 0.0f, 10.0f);
            }

            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<FixedJointComponent>();
            }
            ImGui::PopID(); // End Unique ID
        }

        if (entity.HasComponent<SpringJointComponent>())
        {
            auto& springJoint = entity.GetComponent<SpringJointComponent>();
            bool isCollapsingHeaderOpen = true;
            ImGui::PushID("SpringJoint"); // Unique ID
            if (ImGui::CollapsingHeader("Spring Joint", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Connected Body Field
                ImGui::Text("Connected Body");

               
                if (ImGui::Button(springJoint.ConnectedBody[0] != '\0' ? springJoint.ConnectedBody : "None (Rigidbody)",
                                  ImVec2(200, 20)))
                {
                   
                    strcpy(springJoint.ConnectedBody, ""); 
                }

                
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RIGIDBODY"))
                    {
                        const char* droppedObjectName = (const char*)payload->Data;
                        strncpy(springJoint.ConnectedBody, droppedObjectName, sizeof(springJoint.ConnectedBody) - 1);
                        springJoint.ConnectedBody[sizeof(springJoint.ConnectedBody) - 1] =
                            '\0'; 
                    }
                    ImGui::EndDragDropTarget();
                }

                // Anchor
                ImGui::Text("Anchor");
                ImGui::DragFloat3("##Anchor", (float*)&springJoint.Anchor, 0.1f);

                // Auto Configure Connected Anchor
                ImGui::Checkbox("Auto Configure Connected Anchor", &springJoint.AutoConfigureConnectedAnchor);

                // Connected Anchor
                if (!springJoint.AutoConfigureConnectedAnchor)
                {
                    ImGui::Text("Connected Anchor");
                    ImGui::DragFloat3("##ConnectedAnchor", (float*)&springJoint.ConnectedAnchor, 0.1f);
                }

                // Spring
                ImGui::Text("Spring");
                ImGui::DragFloat("##Spring", &springJoint.Spring, 1.0f, 0.0f, FLT_MAX);

                // Damper
                ImGui::Text("Damper");
                ImGui::DragFloat("##Damper", &springJoint.Damper, 1.0f, 0.0f, FLT_MAX);

                // Min Distance
                ImGui::Text("Min Distance");
                ImGui::DragFloat("##MinDistance", &springJoint.MinDistance, 0.1f, 0.0f, FLT_MAX);

                // Max Distance
                ImGui::Text("Max Distance");
                ImGui::DragFloat("##MaxDistance", &springJoint.MaxDistance, 0.1f, 0.0f, FLT_MAX);

                // Tolerance
                ImGui::Text("Tolerance");
                ImGui::DragFloat("##Tolerance", &springJoint.Tolerance, 0.001f, 0.0f, 1.0f);

                // Break Force
                ImGui::Text("Break Force");
                ImGui::DragFloat("##BreakForce", &springJoint.BreakForce, 1.0f, 0.0f, FLT_MAX);

                // Break Torque
                ImGui::Text("Break Torque");
                ImGui::DragFloat("##BreakTorque", &springJoint.BreakTorque, 1.0f, 0.0f, FLT_MAX);

                // Enable Collision
                ImGui::Checkbox("Enable Collision", &springJoint.EnableCollision);

                // Enable Preprocessing
                ImGui::Checkbox("Enable Preprocessing", &springJoint.EnablePreprocessing);

                // Mass Scale
                ImGui::Text("Mass Scale");
                ImGui::DragFloat("##MassScale", &springJoint.MassScale, 0.1f, 0.0f, 10.0f);

                // Connected Mass Scale
                ImGui::Text("Connected Mass Scale");
                ImGui::DragFloat("##ConnectedMassScale", &springJoint.ConnectedMassScale, 0.1f, 0.0f, 10.0f);
            }

            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<SpringJointComponent>();
            }
            ImGui::PopID();
        }

        if (entity.HasComponent<DistanceJoint2DComponent>())
        {
            auto& distanceJoint = entity.GetComponent<DistanceJoint2DComponent>();

            bool isCollapsingHeaderOpen = true;
            ImGui::PushID("DistanceJoint2D"); // Unique ID
            if (ImGui::CollapsingHeader("Distance Joint 2D", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Connected Rigidbody Field
                ImGui::Text("Connected Rigidbody");

                // Draggable target area
                if (ImGui::Button(distanceJoint.ConnectedRigidbody[0] != '\0' ? distanceJoint.ConnectedRigidbody
                                                                              : "None (Rigidbody 2D)",
                                  ImVec2(200, 20)))
                {
                    // Handle button click event (e.g., clear binding)
                    strcpy(distanceJoint.ConnectedRigidbody, ""); // Clear binding
                }

                // Accept drag and drop target
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RIGIDBODY2D"))
                    {
                        const char* droppedObjectName = (const char*)payload->Data;
                        strncpy(distanceJoint.ConnectedRigidbody, droppedObjectName,
                                sizeof(distanceJoint.ConnectedRigidbody) - 1);
                        distanceJoint.ConnectedRigidbody[sizeof(distanceJoint.ConnectedRigidbody) - 1] =
                            '\0'; // Ensure string is null-terminated
                    }
                    ImGui::EndDragDropTarget();
                }

                // Enable Collision
                ImGui::Checkbox("Enable Collision", &distanceJoint.EnableCollision);

                // Auto Configure Connected Anchor
                ImGui::Checkbox("Auto Configure Connected Anchor", &distanceJoint.AutoConfigureConnectedAnchor);

                // Anchor
                ImGui::Text("Anchor");
                ImGui::DragFloat2("##Anchor", (float*)&distanceJoint.Anchor, 0.1f);

                // Connected Anchor
                ImGui::Text("Connected Anchor");
                ImGui::DragFloat2("##ConnectedAnchor", (float*)&distanceJoint.ConnectedAnchor, 0.1f);

                // Auto Configure Distance
                ImGui::Checkbox("Auto Configure Distance", &distanceJoint.AutoConfigureDistance);

                // Distance
                ImGui::Text("Distance");
                ImGui::DragFloat("##Distance", &distanceJoint.Distance, 0.1f, 0.0f, FLT_MAX);

                // Max Distance Only
                ImGui::Checkbox("Max Distance Only", &distanceJoint.MaxDistanceOnly);

                // Break Action
                ImGui::Text("Break Action");
                // Assuming there are predefined break actions to choose from
                static const char* breakActions[] = {"None", "Destroy", "Disable"};
                static int selectedAction = 0; // Default selection
                ImGui::Combo("##BreakAction", &selectedAction, breakActions, IM_ARRAYSIZE(breakActions));
                distanceJoint.BreakAction = selectedAction; // Save selected break action

                // Break Force
                ImGui::Text("Break Force");
                ImGui::DragFloat("##BreakForce", &distanceJoint.BreakForce, 1.0f, 0.0f, FLT_MAX);
            }

            // Remove component if header is not open
            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<DistanceJoint2DComponent>();
            }

            ImGui::PopID();
        }

        if (entity.HasComponent<SliderJoint2DComponent>())
        {
            auto& sliderJoint = entity.GetComponent<SliderJoint2DComponent>();

            bool isCollapsingHeaderOpen = true;
            ImGui::PushID("SliderJoint2D"); // Unique ID
            if (ImGui::CollapsingHeader("Slider Joint 2D", &isCollapsingHeaderOpen, ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Connected Rigidbody Field
                ImGui::Text("Connected Rigidbody");

                // Draggable target area
                if (ImGui::Button(sliderJoint.ConnectedRigidbody[0] != '\0' ? sliderJoint.ConnectedRigidbody
                                                                            : "None (Rigidbody 2D)",
                                  ImVec2(200, 20)))
                {
                    // Handle button click event (e.g., clear binding)
                    strcpy(sliderJoint.ConnectedRigidbody, ""); // Clear binding
                }

                // Accept drag and drop target
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RIGIDBODY2D"))
                    {
                        const char* droppedObjectName = (const char*)payload->Data;
                        strncpy(sliderJoint.ConnectedRigidbody, droppedObjectName,
                                sizeof(sliderJoint.ConnectedRigidbody) - 1);
                        sliderJoint.ConnectedRigidbody[sizeof(sliderJoint.ConnectedRigidbody) - 1] =
                            '\0'; // Ensure string is null-terminated
                    }
                    ImGui::EndDragDropTarget();
                }

                // Enable Collision
                ImGui::Checkbox("Enable Collision", &sliderJoint.EnableCollision);

                // Anchor
                ImGui::Text("Anchor");
                ImGui::DragFloat2("##Anchor", (float*)&sliderJoint.Anchor, 0.1f);

                // Connected Anchor
                ImGui::Text("Connected Anchor");
                ImGui::DragFloat2("##ConnectedAnchor", (float*)&sliderJoint.ConnectedAnchor, 0.1f);

                // Angle
                ImGui::Text("Angle");
                ImGui::DragFloat("##Angle", &sliderJoint.Angle, 0.1f, -360.0f, 360.0f);

                // Use Motor
                ImGui::Checkbox("Use Motor", &sliderJoint.UseMotor);

                if (sliderJoint.UseMotor)
                {
                    // Motor Speed
                    ImGui::Text("Motor Speed");
                    ImGui::DragFloat("##MotorSpeed", &sliderJoint.MotorSpeed, 0.1f, -FLT_MAX, FLT_MAX);

                    // Max Motor Force
                    ImGui::Text("Max Motor Force");
                    ImGui::DragFloat("##MaxMotorForce", &sliderJoint.MaxMotorForce, 0.1f, 0.0f, FLT_MAX);
                }

                // Use Limits
                ImGui::Checkbox("Use Limits", &sliderJoint.UseLimits);

                if (sliderJoint.UseLimits)
                {
                    // Min Translation
                    ImGui::Text("Min Translation");
                    ImGui::DragFloat("##MinTranslation", &sliderJoint.MinTranslation, 0.1f, -FLT_MAX, FLT_MAX);

                    // Max Translation
                    ImGui::Text("Max Translation");
                    ImGui::DragFloat("##MaxTranslation", &sliderJoint.MaxTranslation, 0.1f, -FLT_MAX, FLT_MAX);
                }

                // Break Action
                ImGui::Text("Break Action");
                static const char* breakActions[] = {"None", "Destroy", "Disable"};
                static int selectedAction = 0; // Default selection
                ImGui::Combo("##BreakAction", &selectedAction, breakActions, IM_ARRAYSIZE(breakActions));
                sliderJoint.BreakAction = selectedAction; // Save selected break action

                // Break Force
                ImGui::Text("Break Force");
                ImGui::DragFloat("##BreakForce", &sliderJoint.BreakForce, 1.0f, 0.0f, FLT_MAX);

                // Break Torque
                ImGui::Text("Break Torque");
                ImGui::DragFloat("##BreakTorque", &sliderJoint.BreakTorque, 1.0f, 0.0f, FLT_MAX);
            }

            // Remove component if header is not open
            if (!isCollapsingHeaderOpen)
            {
                entity.RemoveComponent<SliderJoint2DComponent>();
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        float buttonWidth = 200.0f;
        float buttonHeight = 32.0f;
        float availableWidth = ImGui::GetContentRegionAvail().x;
        float cursorPosX = (availableWidth - buttonWidth) * 0.5f;
        ImGui::SetCursorPosX(cursorPosX);

        if (ImGui::Button("Add Component", {buttonWidth, buttonHeight}))
        {
            ImGui::OpenPopup("Add Component...");
        }

        if (ImGui::BeginPopupModal("Add Component..."))
        {
            static char buffer[256] = "";
            ImGui::InputTextWithHint("##Search Component", "Search Component:", buffer, 256);

            std::string items[] = {
                "Tag Component",
                "Transform Component",
                "Mesh Component",
                "Material Component",
                "Light Component",
                "Camera Component",
                "Lua Script Component",
                "Rigidbody Component",
                "BoxCollider Component",
                "SphereCollider Component",
                "CapsuleCollider Component",
                "CylinderCollider Component",
                "PlaneCollider Component",
                "MeshCollider Component",
                "Distance2DJoint Component",
                "FixedJoint Component",
                "SpringJoint Component",
                "SlideJoint Component",
                "AxisJoint Component",
                "CustomJoint Component"};
            static int item_current = 1;

            if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y - 200)))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_current == n);
                    if (ImGui::Selectable(items[n].c_str(), is_selected))
                        item_current = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            ImGui::Text("Description");
            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras vel odio lectus. Integer "
                               "scelerisque lacus a elit consequat, at imperdiet felis feugiat. Nunc rhoncus nisi "
                               "lacinia elit ornare, eu semper risus consectetur.");

            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Add Component"))
            {
                if (items[item_current] == "Tag Component")
                {
                    if (!entity.HasComponent<TagComponent>())
                        entity.AddComponent<TagComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Transform Component")
                {
                    if (!entity.HasComponent<TransformComponent>())
                        entity.AddComponent<TransformComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Mesh Component")
                {
                    if (!entity.HasComponent<MeshComponent>())
                        entity.AddComponent<MeshComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Material Component")
                {
                    if (!entity.HasComponent<MaterialComponent>())
                        entity.AddComponent<MaterialComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Light Component")
                {
                    if (!entity.HasComponent<LightComponent>())
                        entity.AddComponent<LightComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Camera Component")
                {
                    if (!entity.HasComponent<CameraComponent>())
                        entity.AddComponent<CameraComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Script Component")
                {
                    if (!entity.HasComponent<ScriptComponent>())
                        // entity.AddComponent<ScriptComponent>();
                        //  TODO add script component
                        ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Rigidbody Component")
                {
                    if (!entity.HasComponent<RigidbodyComponent>())
                    {
                        auto& t = entity.GetComponent<TransformComponent>();
                        entity.AddComponent<RigidbodyComponent>(t);
                    }
                    ImGui::CloseCurrentPopup();
                }
                // Collider
                else if (items[item_current] == "BoxCollider Component")
                {
                    if (!entity.HasComponent<BoxColliderComponent>())
                    {
                        auto& t = entity.GetComponent<TransformComponent>();
                        entity.AddComponent<BoxColliderComponent>(t);
                    }
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "SphereCollider Component")
                {
                    if (!entity.HasComponent<SphereColliderComponent>())
                        entity.AddComponent<SphereColliderComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "CapsuleCollider Component")
                {
                    if (!entity.HasComponent<CapsuleColliderComponent>())
                        entity.AddComponent<CapsuleColliderComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "CylinderCollider Component")
                {
                    if (!entity.HasComponent<CylinderColliderComponent>())
                        entity.AddComponent<CylinderColliderComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "PlaneCollider Component")
                {
                    if (!entity.HasComponent<PlaneColliderComponent>())
                        entity.AddComponent<PlaneColliderComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "MeshCollider Component")
                {
                    if (!entity.HasComponent<MeshColliderComponent>())
                        entity.AddComponent<MeshColliderComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Distance2DJoint Component")
                {
                    if (!entity.HasComponent<DistanceJoint2DComponent>())
                        entity.AddComponent<DistanceJoint2DComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "FixedJoint Component")
                {
                    if (!entity.HasComponent<FixedJointComponent>())
                        entity.AddComponent<FixedJointComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "SpringJoint Component")
                {
                    if (!entity.HasComponent<SpringJointComponent>())
                        entity.AddComponent<SpringJointComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "SlideJoint Component")
                {
                    if (!entity.HasComponent<SliderJoint2DComponent>())
                        entity.AddComponent<SliderJoint2DComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "AxisJoint Component")
                {
                    if (!entity.HasComponent<SpringJointComponent>())
                        entity.AddComponent<SpringJointComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "CustomJoint Component")
                {
                    if (!entity.HasComponent<SpringJointComponent>())
                        entity.AddComponent<SpringJointComponent>();
                    ImGui::CloseCurrentPopup();
                }
                else
                {
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

    // UI functions for scenetree menus
    void SceneTreePanel::ShowCreateEntityMenu()
    {
        if (ImGui::BeginPopupModal("Add Entity..."))
        {
            static char buffer[256] = "";
            ImGui::InputTextWithHint("##Search Component", "Search Component:", buffer, 256);

            std::string items[] = {"Empty", "Camera", "Primitive", "Light"};
            static int item_current = 1;

            if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, ImGui::GetContentRegionAvail().y - 200)))
            {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                {
                    const bool is_selected = (item_current == n);
                    if (ImGui::Selectable(items[n].c_str(), is_selected))
                        item_current = n;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndListBox();
            }

            ImGui::Text("Description");
            ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras vel odio lectus. Integer "
                               "scelerisque lacus a elit consequat, at imperdiet felis feugiat. Nunc rhoncus nisi "
                               "lacinia elit ornare, eu semper risus consectetur.");

            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Add Component"))
            {
                if (items[item_current] == "Empty")
                {
                    Entity e = m_Context->CreateEntity();
                    SetSelectedEntity(e);
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Camera")
                {
                    Entity e = m_Context->CreateEntity("Camera");
                    e.AddComponent<CameraComponent>();
                    SetSelectedEntity(e);
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Primitive")
                {
                    Entity e = m_Context->CreateEntity("Primitive");
                    e.AddComponent<MeshComponent>();
                    e.AddComponent<MaterialComponent>();
                    SetSelectedEntity(e);
                    ImGui::CloseCurrentPopup();
                }
                else if (items[item_current] == "Light")
                {
                    Entity e = m_Context->CreateEntity("Light");
                    e.AddComponent<LightComponent>();
                    SetSelectedEntity(e);
                    ImGui::CloseCurrentPopup();
                }
                else
                {
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
    }

} // namespace Coffee
