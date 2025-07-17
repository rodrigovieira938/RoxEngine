#include "flecs.h"
#include "flecs/addons/cpp/c_types.hpp"
#include "imgui.h"
#include <RoxEngine/ecs/ecs.hpp>
#include <RoxEngine/imgui/imgui.hpp>
#include <format>
#include <latch>

namespace RoxEngine {
    flecs::world init_world() {
        flecs::world world;
        auto scope = world.scope(world.entity("RoxEngine").add(flecs::Module));
        scope.entity("components").add(flecs::Module);
        scope.entity("cpp").add(flecs::Module);
        return world;
    }
    static flecs::world world = init_world();
    Entity::Entity(uint64_t id) : mId(id) {
    }
    bool Entity::exists() {
        if(mId == 0) return false;
        return world.exists(mId);
    }
    void Entity::destroy() {
        return flecs::entity(world, mId).destruct();
    }
    UntypedComponent::UntypedComponent(uint64_t id) : Entity(id) {}
    size_t UntypedComponent::getSize() {
        //FIXME: use the c++ api if possible
        return ecs_get_type_info(world, mId)->size;
    }
    size_t UntypedComponent::getAlignment() {
        //FIXME: use the c++ api if possible
        return ecs_get_type_info(world, mId)->alignment;
    }

    Scene::Scene(uint64_t id) : mId(id) {
    }
    bool Scene::exists() {
        if(mId == 0) return false;
        return world.exists(mId);
    }
    void Scene::destroy() {
        return flecs::entity(world, mId).destruct();
    }
    Entity Scene::entity() {
        return Entity(world.entity());
    }

    Scene World::createScene() {
        return Scene(reinterpret_cast<uint64_t>(world.prefab().raw_id()));
    }
    UntypedComponent World::lookupComponent(const char* name) {
        //TODO: check if its a component, make aliasses 
        return UntypedComponent(world.scope("RoxEngine::components").lookup(name).raw_id());
    }
    UntypedComponent World::component(const char* name) {
        //TODO: panic if exists
        char * ptr = strtok((char*)name, "::");
        auto lastEntity = world.entity("RoxEngine::components");
        while(ptr != NULL) {
            lastEntity = world.component(ptr).child_of(lastEntity).add(flecs::Module);
            ptr = strtok(NULL, "::");
        }
        //TODO: add type info
        return UntypedComponent(lastEntity.remove(flecs::Module).raw_id());
    }
    void iterate_children(flecs::entity e, flecs::entity* selected) {
        auto name = e.name();
        std::string tree_name;
        if(name == "") {
            tree_name = "Unnamed";
        } else {
            tree_name = std::format("{}##{}", name.c_str(), "").c_str();
        }
        auto c =  *selected == e ? ImGuiTreeNodeFlags_Selected : 0 | ImGuiTreeNodeFlags_OpenOnArrow;
        bool open = ImGui::TreeNodeEx(tree_name.c_str(),ImGuiTreeNodeFlags_OpenOnDoubleClick);
        if(ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            *selected = e;
        }
        if(open) {
            e.children([selected](flecs::entity e) {
                iterate_children(e, selected);
            });
            ImGui::TreePop();
        }
    }

    static flecs::entity selected_entity;
    void World::debugView() {
        if(ImGui::Begin("ECS - Debug View")) {
            world.children([](flecs::entity e) {
                iterate_children(e,&selected_entity);
            });
            ImGui::End();
        }
        if(ImGui::Begin("Inspector")) {
            if(selected_entity) {
                ImGui::Text("%s",selected_entity.name().c_str());
                selected_entity.each([](flecs::id e){
                    //TODO: skip redundant builtin pairs like Identifier - Name (pair to the entity)
                    if(e.is_pair()) {
                        ImGui::Text("Pair: ");
                        ImGui::SameLine();
                        ImGui::Text("%s", e.first().name().c_str());
                        ImGui::SameLine();
                        ImGui::Text("%s", e.second().name().c_str());
                        return;
                    }
                    ImGui::Text("Component: ");
                    ImGui::SameLine();
                    ImGui::Text("%s",e.entity().name().c_str());
                });
            }
            ImGui::End();
        }
    }
}