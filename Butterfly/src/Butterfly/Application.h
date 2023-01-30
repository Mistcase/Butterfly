#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "LayerStack.h"
#include "Window.h"

#include "Butterfly/Renderer/Shader.h"

#include "ImGui/ImGuiLayer.h"

class WindowCloseEvent;

namespace butterfly
{
    class Camera;
    class IndexBuffer;
    class VertexBuffer;
    class VertexArray;

	class BUTTERFLY_API Application
	{
	public:
        Application(const std::string& name = "App");
		virtual ~Application();

		void run();
        void quit();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

	    inline static Application& GetInstance() { return *m_instance; }
		inline Window& getWindow() { return *m_window; }

	private:
		bool onWindowClose(WindowCloseEvent& e);
        bool onWindowResize(WindowResizeEvent& e);

	private:
		static Application* m_instance;
		std::unique_ptr<Window> m_window;

        LayerStack m_layerStack;
		ImGuiLayer* m_imGuiLayer;

    private:
        bool m_running = true;
        float m_time = 0.0f;
	};

	// Have to be defined by client
	Application* CreateApplication();

} // namespace butterfly