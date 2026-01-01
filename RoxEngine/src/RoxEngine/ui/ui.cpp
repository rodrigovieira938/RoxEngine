#include <cassert>
#include <cstdlib>
#include <iostream>
#include <stack>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <RoxEngine/ui/ui.hpp>
#include <RoxEngine/core/Engine.hpp>

namespace RoxEngine::UI {
    void Layer::Init() {
    }
    void Layer::NewFrame() {
    }
    void Layer::Render() {
    }
    void Layer::Shutdown() {
    }


    struct Element {
        std::string str = "";
        uint32_t children_count = 0;
    };

    class Tree {
    public:
        Tree() {}
        ~Tree() {
            iterate([](Element elem, int depth){
                std::cout << std::string(depth, '\t') << "Begin(\"" << elem.str << "\")"  << "\n";
            });
        }
        template<typename Func>
        void iterate(Func callback) {
            if (mElements.empty()) return;
            Element elem;
            int i = 0;
            for(int i = 0; i < mElements.size(); i += 1 + elem.children_count) {
                elem = mElements.at(i);
                iterate(i, 0, callback);
            }
        }
        template<typename... Args>
        inline auto insert(Args&&... args) {
            mElements.emplace_back(std::forward<Args>(args)...);
            return mElements.size()-1;
        }
        inline auto& at(size_t n) {
            return mElements.at(n);
        }
    private:
        template<typename Func>
        void iterate(size_t index, uint32_t depth, Func callback) {
            auto elem = mElements.at(index);
            if constexpr (std::is_invocable_v<Func, Element, size_t>) {
                callback(elem, depth);
            } else if (std::is_invocable_v<Func, Element&&>) {
                callback(elem);
            } else {
                static_assert(false, "Tree:Iterate callback must be called with (Element, size_t) or (Element)");
            }

            uint32_t index_offset = index + 1;
            for (uint32_t i = 0; i < elem.children_count; ++i) {
                iterate(index_offset+i, depth + 1, callback);
            }
        }

        std::vector<Element> mElements;
    };


    Tree tree;
    std::stack<size_t> treeIndexes;
    
    void BeginElement() {
        if(!treeIndexes.empty()){
            auto index = treeIndexes.top();
            auto& parent = tree.at(index);
            parent.children_count+=1;                
        }
    
        auto element_index = tree.insert();
        tree.at(element_index).str = std::string(std::to_string(element_index+1));
        treeIndexes.push(element_index);
    }
    void EndElement() {
        assert(treeIndexes.size() != 0);
        treeIndexes.pop();
    }
}