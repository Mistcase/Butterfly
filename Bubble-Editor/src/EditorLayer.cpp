#include "EditorLayer.h"

#include <ImGui/imgui.h>

EditorLayer::EditorLayer()
    : Layer("EditorLayer")
{
}

void EditorLayer::onAttach()
{
    auto& window = bubble::Application::GetInstance().getWindow();

    m_camera = std::make_unique<bubble::Camera>(window.getWidth(), window.getHeight());
    m_cameraController = std::make_unique<bubble::CameraController>(m_camera.get());

    m_texture = bubble::Texture2D::Create("assets/Checkerboard.png");
    m_texture2 = bubble::Texture2D::Create("assets/smoke.png");

    bubble::FramebufferSpecification fbSpec;
    fbSpec.width = 1280;
    fbSpec.height = 720;
    m_framebuffer = bubble::Framebuffer::Create(fbSpec);
}

void EditorLayer::onDetach()
{
}

void EditorLayer::onUpdate(float dt)
{
    m_cameraController->onUpdate(dt);

    m_framebuffer->bind();

    bubble::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    bubble::RenderCommand::Clear();

    bubble::Renderer2D::BeginScene(m_camera.get());
    bubble::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 250.0f, 250.0f }, glm::radians(45.0f), { 1.0f, 0.0f, 0.0f, 1.0f }, m_texture);
    bubble::Renderer2D::DrawQuad({ 300.0f, 300.0f, -0.1f }, { 250.0f, 250.0f }, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f }, m_texture2 );
    bubble::Renderer2D::DrawQuad({ -300.0f, -300.0f, -0.1f }, { 250.0f, 250.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });

    bubble::Renderer2D::EndScene();
    m_framebuffer->unbind();
}

void EditorLayer::onImGuiRender()
{
    static bool opt_fullscreen_persistant = true;
    static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
    bool opt_fullscreen = opt_fullscreen_persistant;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
        window_flags |= ImGuiWindowFlags_NoBackground;

    static bool p_open = true;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &p_open, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Dockspace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Quit", NULL, false, true))
                bubble::Application::GetInstance().quit();
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::Begin("Scene");

    const auto contentRegion = ImGui::GetContentRegionAvail();
    const glm::vec2 viewportSize{ contentRegion.x, contentRegion.y };

    if (viewportSize.x > 0.0f && viewportSize.y > 0.0f)
    {
        if (m_viewportSize != viewportSize)
        {
            const auto width = static_cast<uint32_t>(viewportSize.x);
            const auto height = static_cast<uint32_t>(viewportSize.y);

            m_framebuffer->resize(width, height);
            m_viewportSize = viewportSize;
        }
    }

    const auto textureId = m_framebuffer->getColorAttachmentRendererId();
    ImGui::Image((void*)textureId, { m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
    ImGui::End();

    ImGui::End();
}

void EditorLayer::onEvent(bubble::Event& event)
{
    m_cameraController->onEvent(event);

    bubble::EventDispatcher dispatcher(event);
    dispatcher.dispatch<bubble::KeyReleasedEvent>([this](bubble::KeyEvent& e){ return onKeyEvent(e); });
    dispatcher.dispatch<bubble::WindowResizeEvent>([this](bubble::WindowResizeEvent& e)
    {
        m_camera->setSize(e.getWidth(), e.getHeight());
        return true;
    });
}

bool EditorLayer::onKeyEvent(const bubble::KeyEvent& event)
{
    if (event.getKeyCode() == BUBBLE_KEY_ESCAPE)
    {
        bubble::Application::GetInstance().quit();
        return true;
    }

    return false;
}
