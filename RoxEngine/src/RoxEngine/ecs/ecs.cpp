#include "RoxEngine/core/Logger.hpp"
#include "flecs.h"
#include "flecs/addons/cpp/c_types.hpp"
#include "imgui.h"
#include <RoxEngine/ecs/ecs.hpp>
#include <RoxEngine/imgui/imgui.hpp>
#include <format>

namespace RoxEngine {
    struct ConversionFactor {
        float multiplier;
    };
    flecs::entity ConvertsTo;
    float convert(float value, flecs::entity from, flecs::entity to) {
        if (auto conv = from.get_ref<ConversionFactor>(from.world().pair(ConvertsTo,to))) {
            return value * conv->multiplier;
        }
        // No conversion found — fallback, error, or identity
        return value;
    }
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
    bool Entity::hasComponent(UntypedComponent component) {
        return flecs::entity(world, mId).has(component.mId);
    }
    void* Entity::addComponent(UntypedComponent component) {
        auto e = flecs::entity(world, mId); 
        e.add(component.mId);
        return e.get_mut(component.mId);
    }
    void* Entity::getComponent(UntypedComponent component) {
        return flecs::entity(world, mId).get_mut(component.mId);
    }
    void Entity::removeComponent(UntypedComponent component) {
        flecs::entity(world, mId).remove(component.mId);
    }
    UntypedComponent::UntypedComponent(uint64_t id) : Entity(id) {}
    UntypedComponent::TypeInfo UntypedComponent::getTypeInfo() {
        auto type_info = ecs_get_type_info(world, mId); 
        //FIXME: use the c++ api if possible
        return TypeInfo{(size_t)type_info->size, (size_t)type_info->component};
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
    Entity Scene::entity(const std::string& name) {
        return Entity(world.entity(name.data()).child_of(mId));
    }

    Scene World::createScene(const std::string& name) {
        return Scene(reinterpret_cast<uint64_t>(world.prefab(name.data()).raw_id()));
    }
    UntypedComponent World::lookupComponent(const char* name) {
        //TODO: check if its a component, make aliasses 
        return UntypedComponent(world.scope("RoxEngine::components").lookup(name).raw_id());
    }
    UntypedComponent World::component(const char* name, UntypedComponent::TypeInfo info, UntypedComponent::Hooks hooks) {
        //TODO: panic if exists
        char * ptr = strtok((char*)name, "::");
        auto lastEntity = world.entity("RoxEngine::components");
        while(ptr != NULL) {
            lastEntity = world.component(ptr).child_of(lastEntity).add(flecs::Module);
            ptr = strtok(NULL, "::");
        }
        lastEntity.set<EcsComponent>(EcsComponent{(ecs_size_t)info.size,(ecs_size_t) info.alignment}).remove(flecs::Module);
        ecs_type_hooks_t h={};
        memset(&h, 0, sizeof(ecs_type_hooks_t));
        /*
            xtor ctor = nullptr, dtor = nullptr;
            hook2 move = nullptr, copy = nullptr, copy_ctor = nullptr, move_ctor = nullptr;
            equal_hook equal = nullptr;
        */
        h.ctor = (ecs_xtor_t)hooks.ctor;
        h.dtor = (ecs_xtor_t)hooks.dtor;
        h.move = (ecs_move_t)hooks.move;
        h.copy = (ecs_copy_t)hooks.copy;
        h.move_ctor = (ecs_move_t)hooks.move_ctor;
        h.copy_ctor = (ecs_copy_t)hooks.copy_ctor;
        h.equals = (ecs_equals_t)hooks.equals;
        h.flags = ECS_TYPE_HOOK_CTOR|ECS_TYPE_HOOK_DTOR|ECS_TYPE_HOOK_COPY|ECS_TYPE_HOOK_MOVE|ECS_TYPE_HOOK_COPY_CTOR|ECS_TYPE_HOOK_MOVE_CTOR|ECS_TYPE_HOOK_EQUALS;
        ecs_set_hooks_id(world, lastEntity, &h);
        return UntypedComponent(lastEntity.raw_id());
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