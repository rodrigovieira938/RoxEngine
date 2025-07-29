#include "RoxEngine/core/Logger.hpp"
#include "flecs.h"
#include "imgui.h"
#include <RoxEngine/ecs/ecs.hpp>
#include <RoxEngine/imgui/imgui.hpp>
#include <cstdint>
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
    std::string_view Entity::name() {
        return ecs_get_name(world, mId);
    }
    void Entity::name(std::string_view name) {
        ecs_set_name(world, mId, std::string(name).c_str());
    }

    bool Entity::exists() {
        if(mId == 0) return false;
        return world.exists(mId);
    }
    void Entity::destroy() {
        return flecs::entity(world, mId).destruct();
    }
    UntypedRelation Entity::addRelation(UntypedComponent tag, Entity target) {
        flecs::entity(world, mId).add(world.pair(tag.mId, target.mId));
        return getRelation(tag, target);
    }
    bool Entity::hasRelation(UntypedComponent tag, Entity target) const{
        return flecs::entity(world, mId).has(world.pair(tag.mId, target.mId));
    }
    UntypedRelation Entity::getRelation(UntypedComponent tag, Entity target) {
        //TODO: panic if it doesn't have the component
        return UntypedRelation(tag, target, flecs::entity(world, mId).get_mut(world.pair(tag.mId, target.mId)));
    }
    void Entity::removeRelation(UntypedComponent tag, Entity target) {
        flecs::entity(world, mId).remove(world.pair(tag.mId, target.mId));
    }
    bool Entity::hasComponent(UntypedComponent component) const{
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
    std::string_view UntypedComponent::symbol() {
        return ecs_get_symbol(world, mId);
    }
    void UntypedComponent::symbol(std::string_view symbol) {
        ecs_set_symbol(world, mId, std::string(symbol).c_str());
    }
    UntypedComponent::TypeInfo UntypedComponent::getTypeInfo() {
        auto type_info = ecs_get_type_info(world, mId); 
        //FIXME: use the c++ api if possible
        return TypeInfo{(size_t)type_info->size, (size_t)type_info->component};
    }
    bool UntypedRelation::ok() {
        return target.mId != 0;
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
    Entity Scene::lookup(const std::string& name) {
        return Entity(world.scope(mId).lookup(name.c_str(), "/", "/").raw_id());
    }
    Scene World::createScene(const std::string& name) {
        return Scene(reinterpret_cast<uint64_t>(world.entity(name.data()).raw_id()));
    }
    UntypedComponent World::lookupComponent(const char* name) {
        //TODO: check if its a component, make aliasses 
        return UntypedComponent(world.scope("RoxEngine::components").lookup(name).raw_id());
    }
    UntypedComponent World::lookupComponentBySymbol(const char* symbol) {
        return UntypedComponent(ecs_lookup_symbol(world, symbol, false, false));
    }
    UntypedComponent World::component(const char* name, UntypedComponent::TypeInfo info, UntypedComponent::Hooks hooks) {
        auto component = lookupComponent(name);
        if(component.mId != 0) {
            return component;
        }
        //TODO: panic if exists
        char * ptr = strtok((char*)name, "::");
        auto lastEntity = world.lookup("RoxEngine::components");
        for(;ptr != NULL; ptr = strtok(NULL, "::")) {
            if(auto e = lastEntity.lookup(ptr); e) {
                lastEntity = e;
            } else {
                lastEntity = world.entity(ptr).child_of(lastEntity).add(flecs::Module);
            }
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
        if(info.size != 0)
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
    struct QueryIter::Impl {
        ecs_iter_t iter;
        uint32_t row;
    };
    void* QueryIter::get(int termIndex) {
        auto size = ecs_field_size(&impl->iter, termIndex); 
        if (impl->iter.row_fields & (1llu << termIndex)) {
            return ecs_field_at_w_size(&impl->iter, size, termIndex, impl->row);
        } else {
            bool is_shared = !ecs_field_is_self(&impl->iter, termIndex);
            void* data = ecs_field_w_size(&impl->iter, size, termIndex);
            if(is_shared) //If is shared table only has 1 entry
                return data;
            return (char*)data + (size * impl->row);
        }
    }
    UntypedRelation QueryIter::getRelation(int termIndex) {
        ecs_id_t id = ecs_field_id(&impl->iter, termIndex);
        ecs_entity_t tag = ecs_pair_first(world, id);
        ecs_entity_t target = ecs_pair_second(world, id);
        return UntypedRelation(UntypedComponent(tag), Entity(target), get(termIndex));
    }

    Query::Query(void* query) {
        mQuery = query;
    }
    void Query::each(std::function<void(Entity e, QueryIter& iter)> callback) {
        QueryIter::Impl query_iter{
            ecs_query_iter(world, (ecs_query_t*)mQuery),
            0
        };
        QueryIter iter(&query_iter, this);
        while(ecs_query_next(&query_iter.iter)) {
            for (query_iter.row = 0; query_iter.row < query_iter.iter.count; query_iter.row ++) {
                callback(Entity(query_iter.iter.entities[query_iter.row]), iter);
            }
        }
    }
    Query QueryBuilder::build()  {
        int count = 0;
        for(auto& term : terms) {
            if(term.flags == Term::Flags::GROUP_BEGIN)
                count++;
            else if(term.flags == Term::Flags::GROUP_END)
                count--;
        }
        if(count == 0) {
            //TODO: warn
        }
        if(terms.size() > 32) {
            log::warn("Queries with more than 32 terms are not yet supported!");
            return nullptr;
        }
        ecs_query_desc_t desc = {};
        for (int i = 0; i < terms.size(); i++) {
            const auto& term = terms[i];
            Op op = term.op;
            if(term.component.mId != 0) {
                if(term.op == Op::NOT) {
                    if(i > 0) {
                        if(terms[i-1].op == Op::OR) {
                            log::error("Query cannot negate components inside an OR");
                            return nullptr;
                        }
                    }
                } else if(term.op == Op::OR) {
                    if(i+1 >= terms.size()) {
                        log::error("Query after an or_with it must have an with to finish the group or another or_with to continue the or group.");
                            return nullptr;
                    }
                }

                if(term.flags == Term::Flags::RELATION) {
                    //TODO: add the null entity
                    desc.terms[i].id = world.pair(term.component.mId, (term.target.mId == 0)?flecs::Wildcard:term.target.mId);
                } else {
                    desc.terms[i].id = term.component.mId;
                }
                switch(op) {
                case Op::AND:
                    desc.terms[i].oper = EcsAnd;
                    break;
                case Op::OR:
                    desc.terms[i].oper = EcsOr;
                    break;
                case Op::NOT:
                    desc.terms[i].oper = EcsNot;
                    break;
                }
            } else {
                if(term.flags == Term::Flags::GROUP_BEGIN) {
                    desc.terms[i].id = EcsScopeOpen;
                    desc.terms[i].src.id = EcsIsEntity;
    
                } else if(term.flags == Term::Flags::GROUP_END) {
                    desc.terms[i].id = EcsScopeClose;
                    desc.terms[i].src.id = EcsIsEntity;
                }
            }
        }
        return ecs_query_init(world, &desc);
    }
}