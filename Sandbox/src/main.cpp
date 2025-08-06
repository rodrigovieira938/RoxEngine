#include "RoxEngine/core/Logger.hpp"
#include "RoxEngine/ecs/ecs.hpp"
#include "RoxEngine/slang/slang.hpp"
#include <RoxEngine/RoxEngine.hpp>
#include <glm/glm.hpp>

using namespace RoxEngine;

struct TestGame final : public Game {
    struct TestComponent
    {
        TestComponent() {};
        TestComponent(const std::string& s) { a = s;}
        ~TestComponent() {log::info("~TestComponent()");}
	    bool operator ==(const TestComponent& other) const {return a == other.a;}
        std::string a = "<OOPS>";
    };
    struct ComponentA {
        int a;
	    bool operator ==(const ComponentA& other) const {return true;}
    };
    struct ComponentB {
	    bool operator ==(const ComponentB& other) const {return true;}
    };
    struct ComponentC {
	    bool operator ==(const ComponentC& other) const {return true;}
    };
    struct ComponentD {
	    bool operator ==(const ComponentD& other) const {return true;}
    };
    struct ConvertsTo {
        float factor;
	    bool operator ==(const ConvertsTo& other) const {return factor == other.factor;}
    };
    void TestQuery(Scene& scene) {
        Entity e1 = scene.entity("Entity1");
        Entity e2 = scene.entity("Entity2");
        Entity e3 = scene.entity("Entity3");
        Entity e4 = scene.entity("Entity4");
        Entity e5 = scene.entity("Entity5");
        e1.addComponent<ComponentA>();
        e1.addComponent<ComponentB>();
        e2.addComponent<ComponentA>();
        e2.addComponent<ComponentC>();
        e3.addComponent<ComponentA>();
        e3.addComponent<ComponentD>();
        e4.addComponent<ComponentC>();
        e5.addComponent<ComponentA>();
        e5.addComponent<ComponentB>();
        e5.addComponent<ComponentC>();
        e5.addComponent<ComponentD>();

        auto query = QueryBuilder()
            .with<ComponentA>()
            .without<ComponentB>()
            .or_with<ComponentC>()
            .with<ComponentD>()
            .build();
        query.each([](Entity e, QueryIter& iter){
            //TODO: print name of the entity
            log::info("Match {}", e.name());
        });
    }
    void TestRelations(Scene& scene) {
        auto meter = World::component("meter", {}, {});
        auto centimeter = World::component("centimeter", {}, {});
        centimeter.addRelation<ConvertsTo>(meter, 1.f/100.f);
        meter.addRelation<ConvertsTo>(centimeter, 100.f);

        QueryBuilder()
            .with_relation<ConvertsTo>()
            .build()
            .each([](Entity e, QueryIter& iter){
                auto relation = iter.getRelation(0);
                auto name = relation.getTarget().name();
                auto convertsTo = (ConvertsTo*)relation.get();
                log::info("{} is convertible to {}, conversion factor={}", e.name(), name, convertsTo->factor);
            });

        auto convert = [](float value, UntypedComponent from, UntypedComponent to){
            if(!from.hasRelation<ConvertsTo>(to)) {
                log::error("Cannot convert from {} to {}", from.name(), to.name());
                return 0.f;
            }
            return value * from.getRelation<ConvertsTo>(to)->factor;
        };
        log::info("Converting 100cm to m = {}", convert(100, centimeter, meter));
        centimeter.removeRelation<ConvertsTo>(meter);
        log::info("Converting 50cm to m = {}", convert(50, centimeter, meter));
        log::info("Converting 100m to m = {}", convert(100, meter, centimeter));
        meter.removeRelation<ConvertsTo>(centimeter);
        log::info("Converting 50m to m = {}", convert(50,meter, centimeter));
    }
    void Init() override {
        //Initialize component's friendly name
        World::component<TestComponent>().name("TestGame::TestComponent");
        World::component<ComponentA>().name("TestGame::ComponentA");
        World::component<ComponentB>().name("TestGame::ComponentB");
        World::component<ComponentC>().name("TestGame::ComponentC");
        World::component<ComponentD>().name("TestGame::ComponentD");

        Scene s = World::createScene("TestGameScene");
        
        
        Entity e = s.entity();
        e.name("Entity");
        e.addComponent<TestComponent>("First");
        TestQuery(s);
        TestRelations(s);
        
        s.entity("lookupEntity");
        if(auto e = s.lookup("lookupEntity"); e.exists()) {
            log::info("LookupEntity found");
        }
        SlangLayer::Init();
        auto module = SlangLayer::CompileModule("res://shaders/basic.slang");
        auto moduleReflection = SlangLayer::GetModuleReflection(module);
        
        log::info("Offsets");
        log::info("\tscalarFloat = {}", moduleReflection.lookup("scalarFloat").value_or(-1));
        log::info("\tscalarInt = {}", moduleReflection.lookup("scalarInt").value_or(-1));
        log::info("\tscalarBool = {}", moduleReflection.lookup("scalarBool").value_or(-1));

        // Vectors with component indexing
        log::info("\tvector2 = {}", moduleReflection.lookup("vector2").value_or(-1));
        for (int i = 0; i < 2; ++i) {
            log::info("\t\tvector2[{}] = {}", i, moduleReflection.lookup(std::format("vector2[{}]", i)).value_or(-1));
        }

        log::info("\tvector3 = {}", moduleReflection.lookup("vector3").value_or(-1));
        for (int i = 0; i < 3; ++i) {
            log::info("\t\tvector3[{}] = {}", i, moduleReflection.lookup(std::format("vector3[{}]", i)).value_or(-1));
        }

        log::info("\tvector4 = {}", moduleReflection.lookup("vector4").value_or(-1));
        for (int i = 0; i < 4; ++i) {
            log::info("\t\tvector4[{}] = {}", i, moduleReflection.lookup(std::format("vector4[{}]", i)).value_or(-1));
        }

        // Matrices with column and row indexing
        log::info("\tmatrix3x3 = {}", moduleReflection.lookup("matrix3x3").value_or(-1));
        for (int col = 0; col < 3; ++col) {
            log::info("\t\tmatrix3x3[{}] = {}", col, moduleReflection.lookup(std::format("matrix3x3[{}]", col)).value_or(-1));
            for (int row = 0; row < 3; ++row) {
                log::info("\t\t\tmatrix3x3[{}][{}] = {}", col, row, moduleReflection.lookup(std::format("matrix3x3[{}][{}]", col, row)).value_or(-1));
            }
        }

        log::info("\tmatrix4x4 = {}", moduleReflection.lookup("matrix4x4").value_or(-1));
        for (int col = 0; col < 4; ++col) {
            log::info("\t\tmatrix4x4[{}] = {}", col, moduleReflection.lookup(std::format("matrix4x4[{}]", col)).value_or(-1));
            for (int row = 0; row < 4; ++row) {
                log::info("\t\t\tmatrix4x4[{}][{}] = {}", col, row, moduleReflection.lookup(std::format("matrix4x4[{}][{}]", col, row)).value_or(-1));
            }
        }

        // 1D array
        log::info("\tarray1D = {}", moduleReflection.lookup("array1D").value_or(-1));
        for (int i = 0; i < 5; ++i) {
            log::info("\t\tarray1D[{}] = {}", i, moduleReflection.lookup(std::format("array1D[{}]", i)).value_or(-1));
        }
        // 2D array
        log::info("\tarray2D = {}", moduleReflection.lookup("array2D").value_or(-1));
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j) {
                log::info("\t\tarray2D[{}][{}] = {}", i, j, moduleReflection.lookup(std::format("array2D[{}][{}]", i, j)).value_or(-1));
            }
        }

        // 3D array
        log::info("\tarray3D = {}", moduleReflection.lookup("array3D").value_or(-1));
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                for (int k = 0; k < 4; ++k) {
                    log::info("\t\tarray3D[{}][{}][{}] = {}", i, j, k, moduleReflection.lookup(std::format("array3D[{}][{}][{}]", i, j, k)).value_or(-1));
                }
            }
        }
    }
    void Update() override {
        if(Input::GetKeyState(Key::W) != KeyState::NONE)
               log::info("W KEY action: {}",static_cast<int>(Input::GetKeyState(Key::W)));
    }
    void Render() override {
        World::debugView();
    }
};

Scope<Game> CreateGame()
{
    return CreateScope<TestGame>();
}

//todo: move constructor to the engine project
int main(int, char**) {
    return Engine::Get()->Run(CreateGame());
}