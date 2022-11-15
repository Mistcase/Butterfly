#include "bubblepch.h"
#include "OpenGLVertexArray.h"

#include "Bubble/Log.h"

#include <glad/glad.h>

namespace bubble
{
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
		switch (type)
		{
		case bubble::ShaderDataType::Float:
		case bubble::ShaderDataType::Float2:
		case bubble::ShaderDataType::Float3:
		case bubble::ShaderDataType::Float4:
        case bubble::ShaderDataType::Mat3:
		case bubble::ShaderDataType::Mat4:
			return GL_FLOAT;

		case bubble::ShaderDataType::Int:
		case bubble::ShaderDataType::Int2:
		case bubble::ShaderDataType::Int3:
		case bubble::ShaderDataType::Int4:
            return GL_UNSIGNED_INT;

		case bubble::ShaderDataType::Bool:
			return GL_BOOL;

		default:
			break;
		}

        BUBBLE_CORE_ASSERT(false, "Unknown shader data type");
        return 0;
    };

    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_rendererId);
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_rendererId);
    }

    void OpenGLVertexArray::addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
    {
        glBindVertexArray(m_rendererId);
        vertexBuffer->bind();

        BUBBLE_CORE_ASSERT(!vertexBuffer->getLayout().getElements().empty(), "Vertex buffer layout is undefined");

        int index = 0;
        const auto& layout = vertexBuffer->getLayout();
        for (const auto& element : layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index,
                                  (GLuint)element.getComponentCount(),
                                  ShaderDataTypeToOpenGLBaseType(element.type),
                                  element.normalized ? GL_TRUE : GL_FALSE,
                                  layout.getStride(),
                                  ((const void*)(size_t)element.offset));
            index++;
        }

        m_vertexBuffers.push_back(vertexBuffer);
    }

    void OpenGLVertexArray::setIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
    {
        glBindVertexArray(m_rendererId);
        indexBuffer->bind();

        m_indexBuffer = indexBuffer;
    }

    void OpenGLVertexArray::bind() const
    {
        glBindVertexArray(m_rendererId);
    }

    void OpenGLVertexArray::unbind() const
    {
        glBindVertexArray(0);
    }

    const std::vector<Ref<VertexBuffer>>& OpenGLVertexArray::getVertexBuffers() const
    {
        return m_vertexBuffers;
    }

    const Ref<IndexBuffer> OpenGLVertexArray::getIndexBuffer() const
    {
        return m_indexBuffer;
    }

} // namespace bubble
