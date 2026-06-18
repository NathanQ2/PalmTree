#pragma once

#include "Layer.h"

#include <vector>

namespace PalmTree {
    class LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        template<class T, typename... Args>
        T* PushLayer(Args&&... args) {
            T* layer = new T(std::forward<Args>(args)...);
            m_Layers.emplace(m_Layers.end() - m_NumOverlays, layer);

            layer->OnAttach();

            return layer;
        }

        template<class T, typename... Args>
        T* PushOverlay(Args&&... args) {
            T* layer = new T(std::forward<Args>(args)...);
            m_Layers.emplace_back(layer);
            m_NumOverlays++;

            layer->OnAttach();

            return layer;
        }

        void DeleteLayer(Layer* layer);

        Layer* GetLayer(int index) const { return m_Layers[index]; }

        std::vector<Layer*>::iterator Begin() { return m_Layers.begin(); }
        std::vector<Layer*>::iterator End() { return m_Layers.end(); }
    private:
        std::vector<Layer*> m_Layers;

        int m_NumOverlays = 0;
    };
}
