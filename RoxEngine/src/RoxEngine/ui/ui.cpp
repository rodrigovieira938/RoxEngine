#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <format>
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

    
    class Tree {
    public:
        struct Element: public ::RoxEngine::UI::Element {
            std::string str = "";
            uint32_t children_count = 0;
            uint32_t width = 0, height = 0;
            std::optional<uint32_t> parent_index;
        };
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
        inline void clear() {
            mElements.clear();
        }
    private:
        template<typename Func>
        void iterate(size_t index, uint32_t depth, Func callback) {
            auto elem = mElements.at(index);
            if constexpr(std::is_invocable_v<Func, size_t>){
                callback(index);
            } else {
                if constexpr (std::is_invocable_v<Func, Element, size_t>) {
                    callback(elem, depth);
                } else if (std::is_invocable_v<Func, Element&&>) {
                    callback(elem);
                } else {
                    static_assert(false, "Tree:Iterate callback must be called with (Element, size_t) or (Element)");
                }
            }
            uint32_t index_offset = index + 1;
            for (uint32_t i = 0; i < elem.children_count; ++i) {
                iterate(index_offset+i, depth + 1, callback);
            }
        }
        std::vector<Element> mElements;
    };

    Tree tree;
    std::optional<uint32_t> currentParentIndex;

    namespace Layouting {
        bool is_horizontal_flowdirection(FlowDirection dir) {
            switch (dir) {
            case FlowDirection::LEFT_TO_RIGHT:
            case FlowDirection::RIGHT_TO_LEFT:
                return true;
            case FlowDirection::TOP_TO_BOTTOM:
            case FlowDirection::BOTTOM_TO_TOP:
                return false;
            }
            assert(false);
        }

        //TODO: share remaining size between child elements
        void GrowElements_CalculateRemainingSize(
            Tree::Element element, 
            decltype(element.width)& remainingWidth,
            decltype(element.width)& remainingHeight,
            bool is_horizontal_flow
        ) {
            remainingWidth = element.width - element.padding.left - element.padding.right;
            remainingHeight = element.height - element.padding.top - element.padding.bottom;
            //Calculate remaining size
            {
                for (uint32_t child_index = 0; child_index < element.children_count; child_index++) {
                    auto& child = tree.at(child_index+1);
                    if(is_horizontal_flow) {
                        remainingWidth -= child.width;
                    } else {
                        remainingHeight -= child.height;
                    }
                }
                auto child_gap = (element.children_count - 1) * element.childGap;
                if(is_horizontal_flow)
                    remainingWidth -= child_gap;
                else
                    remainingHeight -= child_gap;
            }
        }
        void GrowElements(size_t index) {
            auto& element = tree.at(index);
            if(element.width == 0 || element.height == 0 || element.children_count == 0)
                return;
            bool is_horizontal_flow = is_horizontal_flowdirection(element.flowDirection);
            decltype(element.width) totalRemainingWidth = 0;
            decltype(element.height) totalRemainingHeight = 0;
            GrowElements_CalculateRemainingSize(element, totalRemainingWidth, totalRemainingHeight, is_horizontal_flow);

            double widthDivisions = 0;
            double heightDivisions = 0;

            double remainingWidth_divided = totalRemainingWidth;
            double remainingHeight_divided = totalRemainingHeight;

            for (uint32_t child_index = 0; child_index < element.children_count; child_index++) {
                auto& child = tree.at(child_index+1);
                if(child.sizing.width.type == ElementSizing::GROW)
                    widthDivisions+=std::abs(child.sizing.width.growth_rate);
                if(child.sizing.height.type == ElementSizing::GROW)
                    heightDivisions+=std::abs(child.sizing.height.growth_rate);
            }

            if(is_horizontal_flow) {
                remainingWidth_divided = float(totalRemainingWidth) / widthDivisions;
            } else {
                remainingHeight_divided = float(totalRemainingHeight) / heightDivisions;
            }


            for (uint32_t child_index = 0; child_index < element.children_count; child_index++) {
                auto& child = tree.at(child_index+1);
                if(child.sizing.width.type == ElementSizing::GROW) { 
                    uint32_t remainingWidth = std::floor(remainingWidth_divided * child.sizing.width.growth_rate);
                    //Get the minimum size needed
                    auto width = std::min(child.sizing.min_width + remainingWidth, child.sizing.max_width);
                    child.width = std::min(width, totalRemainingWidth);
                }
                if(child.sizing.height.type == ElementSizing::GROW) {
                    uint32_t remainingHeight = std::floor(remainingHeight_divided * child.sizing.height.growth_rate);
                    //Get the minimum size needed
                    auto height = std::min(child.sizing.min_height + remainingHeight, child.sizing.max_height);
                    child.height = std::min(height, totalRemainingHeight);
                }
            }
        }
        void FitElement(Tree::Element& element) {
            auto& padding = element.padding;
            element.width += padding.left + padding.right;
            element.height += padding.top + padding.bottom;
            if(element.parent_index.has_value()) {
                auto& parent = tree.at(element.parent_index.value());
                auto horizontal_flowdirection = is_horizontal_flowdirection(parent.flowDirection);
                static auto fit_element_value = [](auto& parent_axis_size, auto element_axis_size, auto childGap, bool alongAxis){
                    if(alongAxis) {
                        parent_axis_size += element_axis_size + childGap;
                    } else {
                        parent_axis_size = std::max(parent_axis_size, element_axis_size);
                    }
                };
                decltype(element.childGap) childGap = 0;
                if(parent.children_count > 1) {
                    childGap = parent.childGap;
                }
                if(parent.sizing.width.type != ElementSizing::FIXED) {
                    auto width = element.width;
                    if(element.sizing.width.type != ElementSizing::FIXED)
                        width = element.sizing.min_width;
                    fit_element_value(parent.width, width, childGap, horizontal_flowdirection);
                }
                if(parent.sizing.height.type != ElementSizing::FIXED) {
                    auto height = element.height;
                    if(element.sizing.height.type != ElementSizing::FIXED)
                        height = element.sizing.min_height;
                    fit_element_value(parent.height, element.height, childGap, !horizontal_flowdirection);
                }
            }
        }
        void OnEndElement(Tree::Element& el) {
            FitElement(el);
        }
        void OnEndTree() {
            tree.iterate(GrowElements);
            tree.iterate([](Tree::Element elem, int depth){
                std::cout << std::format("{}Node width={} height={} \"{}\"", std::string(depth, '\t'), elem.width, elem.height, elem.str) << "\n";
            });
            tree.clear();
        }
    }
    void BeginElement(Tree::Element el) {
        //If node has parent increment its children_count
        if(currentParentIndex.has_value()){
            auto& parent = tree.at(currentParentIndex.value());
            parent.children_count+=1;                
        }
    
        auto element_index = tree.insert(el);
        auto& element = tree.at(element_index);
        element.parent_index = currentParentIndex;
        element.width = el.sizing.width + el.margin.left + el.margin.right;
        element.height = el.sizing.height + el.margin.top + el.margin.bottom;
        currentParentIndex = element_index;
    }
    void BeginElement(const Element& el) {
        BeginElement(Tree::Element(el));
    }
    void EndElement() {
        auto& element = tree.at(currentParentIndex.value());
        Layouting::OnEndElement(element);
        currentParentIndex = element.parent_index;
        if(!currentParentIndex.has_value()) {
            Layouting::OnEndTree();
        }
    }
    void TextElement(std::string_view str) {
        // lets assume a square font
        int fontSize = 16;

        Tree::Element el;
        el.str = std::string(str);
        
        //Growth rate = 0 because text doesn't need to grow, only shrink to min size.
        el.sizing.width = ElementSizing::grow(0);
        el.sizing.height = ElementSizing::grow(0);

        el.sizing.min_width = ElementSizing::fixed(str.size() * fontSize);
        el.sizing.min_height = fontSize;
        
        //Lets assume the wrap function will wrap by character
        el.sizing.max_width = el.sizing.min_width;
        //We don't need to set max_height since it'll be longer than min: so it can wrap the text
        BeginElement(el);
        EndElement();
    }
}