#include "bubblepch.h"
#include "Application.h"

#include "Log.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

#include "Bubble/Renderer/Renderer.h"
#include "Bubble/Renderer/Camera.h"

#include "Input.h"

#include <imgui.h>

namespace bubble
{
	Application* Application::m_instance = nullptr;

	Application::Application()
		: m_window(Window::Create())
		, m_imGuiLayer(new ImGuiLayer())
	{
		BUBBLE_CORE_ASSERT(!m_instance, "Application already exists");
		m_instance = this;

		m_window->setEventCallback([this](Event& event) { onEvent(event); });

		pushOverlay(m_imGuiLayer);

        m_camera = std::make_unique<Camera>(m_window->getWidth(), m_window->getHeight());
        m_camera->setPosition({ 0.0, 0.0f, 200.0f });
        // m_camera->setRotation(M_PI_4);

        m_vertexArray.reset(VertexArray::Create());
		float vertices[] = {
            300.0f, 100.0f, 0.0f,
            200.0f, 200.0f, 0.0f,
            400.0f, 200.0f, 0.0f,
		};

		unsigned int indices[] = { 0, 1, 2 };
        m_indexBuffer.reset(IndexBuffer::Create(indices, std::size(indices)));
        m_squareVA.reset(VertexArray::Create());
        auto squareVB = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));
        squareVB->setLayout({
                { ShaderDataType::Float3, "a_Position" }
            });

        m_squareVA->addVertexBuffer(squareVB);
        m_squareVA->setIndexBuffer(m_indexBuffer);

        std::string vertexSource = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;

        uniform mat4 u_VP;
        out vec4 v_Color;

        void main()
        {
            v_Color = vec4(0.8, 0.0, 0.0, 1.0);
            gl_Position = u_VP * vec4(a_Position, 1.0);
        }
        )";

        std::string fragmentSource = R"(
        #version 330 core

        layout(location = 0) out vec4 color;

        in vec4 v_Color;

        void main()
        {
            color = v_Color;
        }
        )";

        m_shader = std::make_unique<Shader>(vertexSource, fragmentSource);
	}

	Application::~Application()
	{
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

	void Application::run()
	{
		while (m_running)
		{
            Renderer::BeginScene(m_camera.get(), m_shader.get());
            {
                RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
                RenderCommand::Clear();

                Renderer::Submit(m_squareVA);
            }
            Renderer::EndScene();
            // Renderer::Flush();

			for (auto layer : m_layerStack)
				layer->onUpdate();

			m_imGuiLayer->begin();
			for (auto layer : m_layerStack)
				layer->onImGuiRender();

            if (ImGui::CollapsingHeader("Camera"))
            {
                float rotation = m_camera->getRotation();
                glm::vec3 position = m_camera->getPosition();
                ImGui::InputFloat3("Position", (float*)(&position), 10.0f);
                ImGui::InputFloat("Rotation", (float*)(&rotation), 0.1f);

                m_camera->setPosition(position);
                m_camera->setRotation(rotation);
            }

			m_imGuiLayer->end();

			m_window->onUpdate();
		}
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return onWindowClose(e); });

		for (auto it = std::prev(m_layerStack.end()); it != m_layerStack.begin(); --it)
		{
			(*it)->onEvent(e);
			if (e.handled)
				break;
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		m_layerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* layer)
	{
		m_layerStack.pushOverlay(layer);
		layer->onAttach();
	}

} // namespace bubble
